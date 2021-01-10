// Client side C/C++ program to demonstrate Socket programming 
#include <bits/stdc++.h> 
#include <iostream> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h>
#include <sys/types.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string> 
#include <cstring>
#include <pthread.h>

using namespace std;

#define MSGSIZE 512
#define TRACKER_PORT 6969 

typedef struct myfiledata
{
    string fname;
    string fpath;
    long long fsize;
}myfileinfo;

unordered_map<string , myfileinfo*>     filemap;            //filename->fileinfo struct
unordered_map<string,string>        downloadsmap;
long long chunksize = 65536;            //pkt size to be sent between sender and receiver
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * handleConn(void *param)
{
    int cl_socket = *(int*)param;
    free(param);
    char piece[MSGSIZE] = {0};
    int valread = 1;


    //receive filename
    valread = recv( cl_socket , piece, MSGSIZE ,0);
    string fname = string(piece);

    //open file for transfer
    ifstream in;
    cout<<fname<<"\n";
    string filepath = filemap[fname]->fpath;
    
    const char *fp = filepath.c_str();
    cout<<fp<<"\n";
    in.open(fp);
    cout<<"debugging3\n";

    //send file in chunks
    char * buffer1 = new char [chunksize];
    char * buffer2 = new char [chunksize];
    char * buffer3 = new char [chunksize];
    char * buffer4 = new char [chunksize];
    while(in)
    {
        in.read(buffer1, chunksize);
        in.read(buffer2, chunksize);
        in.read(buffer3, chunksize);
        in.read(buffer4, chunksize);


        unsigned int sent_size;
        sent_size = send(cl_socket , buffer1 , chunksize , 0 );
        sent_size = send(cl_socket , buffer2 , chunksize , 0 );
        sent_size = send(cl_socket , buffer3 , chunksize , 0 );
        sent_size = send(cl_socket , buffer4 , chunksize , 0 );

        //cout<<"sent size "<<sent_size<<"\n";
    }
    delete[] buffer1;
    delete[] buffer2;
    delete[] buffer3;
    delete[] buffer4;

    cout<<"debugging4\n";
    //send empty buffer to indicate connection end
    char * emptybuff = new char [chunksize];
    send(cl_socket , emptybuff , chunksize , 0 );
    delete[] emptybuff;

    cout<<"send complete\n";
    close(cl_socket);

    return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void * clientListener(void *param)
{
    /*
    while(1)
    {
        cout<<"client listening...\n";
        sleep(5);
    }
    */
    

    char *temp = (char*)param;
    string ipport = temp,p1,p2;

    stringstream check1(ipport);
    getline(check1, p1, ':');
    getline(check1, p2, ':');

    char* ip = (char*)p1.c_str();
    char* port = (char*)p2.c_str();

    cout<<ip<<"\n"<<port<<"\n";

    int server_fd, new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[MSGSIZE] = {0}; 


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    }
    address.sin_family = AF_INET; 
    inet_aton(ip, &(address.sin_addr));   //0.0.0.0 listen to all interfaces on our local machine
    address.sin_port = htons(atoi(port));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    }
    if (listen(server_fd, 10) == -1) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    while(1)
    {     //client socket    
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) == -1) 
        { 
            perror("accept"); 
            exit(EXIT_FAILURE); 
        }
        else
        {
            cout<<"new peer connected\n"<<inet_ntoa(address.sin_addr)<<"\n";
        } 
        
        pthread_t th;

        int *clsocket = (int*)malloc(sizeof(int));
        *clsocket = new_socket;

        int rc = pthread_create(&th, NULL, handleConn, (void*)clsocket);
        if (rc){
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
        }

        //pthread_join(th, NULL); 
        //pthread_exit(NULL); 
    }
    return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////function to donload file from  peer////////////////////////////   
void download_from_peer(int port,string ip,string fname,string destination)
{
    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    
    char buffer[MSGSIZE]; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        exit(EXIT_FAILURE); 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(port); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, ip.c_str(), &serv_addr.sin_addr)<=0)         //connect to tracker
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        exit(EXIT_FAILURE); 
    } 
    
    int newconn;
    if (newconn = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        exit(EXIT_FAILURE);
    }

    //send filename to download
    string str = fname;
    char* msg = (char*)str.c_str();
    unsigned int sent_size = send(sock , msg , strlen(msg) , 0 ); 

    //open file to write recv data
    destination.erase(0,1);         //remove first '/' from destination
    destination += fname;           //destination and name of file to be created
    //cout<<destination<<"\n";
    ofstream out;
    out.open(destination, ios::app);

    //start receiving chunks from peer
    char *piece1 = new char[chunksize] ;
    char *piece2 = new char[chunksize] ;
    char *piece3 = new char[chunksize] ;
    char *piece4 = new char[chunksize] ;
    while(1)
    {   
        int valread1 = recv( sock , piece1, chunksize ,0);
        int valread2 = recv( sock , piece2, chunksize ,0);
        int valread3 = recv( sock , piece3, chunksize ,0);
        int valread4 = recv( sock , piece4, chunksize ,0);

        if(valread1 == 0)   break;
        else    out.write(piece1, 65536);

        if(valread2 == 0)   break;
        else    out.write(piece2, 65536);

        if(valread3 == 0)   break;
        else    out.write(piece3, 65536);

        if(valread4 == 0)   break;
        else    out.write(piece4, 65536);

        //cout<<valread<<" bytes written to file\n"; 
    }
    delete[] piece1;
    delete[] piece2;
    delete[] piece3;
    delete[] piece4;

    //cout<<"file downloaded successfully !!!\n";

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////   
int main(int argc, char const *argv[]) 
{ 
    
    pthread_t th;
    int rc = pthread_create(&th, NULL, clientListener,(void *)argv[1]);
    if (rc)
    {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
    }
    

    int sock = 0, valread; 
    struct sockaddr_in serv_addr; 
    
    char buffer[MSGSIZE]; 
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(TRACKER_PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)         //connect to tracker
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
    
    int newconn;
    if (newconn = connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    }

    
    char *temp = (char*)argv[1];
    string ipport = temp,p1,p2;

    stringstream check1(ipport);
    getline(check1, p1, ':');
    getline(check1, p2, ':');

    char* listen_ip = (char*)p1.c_str();
    char* listen_port = (char*)p2.c_str();

    //cout<<p1<<p2<<"\n";
    cout<<listen_ip<<"\n"<<listen_port<<"\n";
    string s;
    do 
    {
        //cout<<listen_port<<" "<<listen_ip;
        getline(cin, s);
        stringstream check1(s);
        string command;
        getline(check1, command, ' ');          //get command name for processing

        if(command == "login")
        {
            
            s = s + " " + listen_ip + " " + listen_port;        //add listen ip port 
            //cout<<s<<"\n";

            char *sendmsg = (char*)s.c_str();
            send(sock , sendmsg , s.size() , 0 ); 
            valread = recv( sock , buffer, MSGSIZE, 0);
            buffer[valread] = '\0' ;
            printf("message receieved: %s\n",buffer );
        }
        else if(command == "download_file")
        {   
            ////////////////////////////////////////////////
            /////////Receive peer data from tracker/////////
            char *sendmsg = (char*)s.c_str();
            send(sock , sendmsg , s.size() , 0 ); 

            valread = recv( sock , buffer, MSGSIZE, 0);

            buffer[valread] = '\0' ;
            string msg_recv = string(buffer);
            cout<<"peer info recieved: "<<msg_recv<<"\n";

            stringstream tokenize(msg_recv);
            string peer_online_status;
            ////////////////////////////////////////////////

            /////////Start downloading from peer////////////
            string gid,fname,dest;
            getline(check1, gid, ' ');              //get gid from command
            getline(check1, fname, ' ');            //get filename from command
            getline(check1, dest, ' ');             //get destination path from command


            getline(tokenize, peer_online_status, ' ');

            if(peer_online_status == "offline")
                cout<<"Peer offline, cannot download\n";

            else if(peer_online_status == "online")
            {
                    
                downloadsmap[fname] = "[D] " + gid + " " + fname;     //File downloading

                string peer_port,peer_ip;
                getline(tokenize, peer_port, ' ');
                getline(tokenize, peer_ip, ' ');
                int peer_port_int = stoi(peer_port);

                cout<<"downloading...\n";
                download_from_peer(peer_port_int,peer_ip,fname,dest);

                
                downloadsmap[fname] = "[C] " + gid + " " + fname;     //File download complete
                cout<<"download complete\n";
            }
        }
        else if(command == "upload_file")           //in uploading we also send the file size
        {   

            string grp,fpath;
            getline(check1, fpath, ' ');            //get command name for processing
            getline(check1, grp, ' ');              //get command name for processing


            //cout<<fpath<<" "<<grp<<"\n";
            //////////store file and its path on client side//////////
            stringstream tokenize(fpath);
            string file_name;
            while(getline(tokenize , file_name, '/')) 
                ;

            myfileinfo *newfile = new myfileinfo();
            //filemap[file_name] = fpath;                 //store filename and path at client side

            //get file size
            ifstream in_file(fpath , ios::binary);
            in_file.seekg(0, ios::end);
            int file_size = in_file.tellg();
            string fsize = to_string(file_size);

            //store file info on client side
            newfile->fname = file_name;
            newfile->fpath = fpath;
            newfile->fsize = file_size;
            filemap[file_name] = newfile;

            cout<<"Info stored on client side: "<<filemap[file_name]->fname
                <<"\nfilepath: "<<filemap[file_name]->fpath
                <<"\nfile size: "<<filemap[file_name]->fsize<<"\n";


            s = s + " " + fsize;                    //append file size to command
            char *sendmsg = (char*)s.c_str();
            send(sock , sendmsg , s.size() , 0 ); 


            valread = recv( sock , buffer, MSGSIZE, 0);
            buffer[valread] = '\0' ;
            string msg_recv = string(buffer);
            cout<<msg_recv<<"\n";         
            
        }
        else if(command == "show_downloads")           
        {
            cout<<"\n";
            for(auto download : downloadsmap)
                cout<<download.second<<"\n";
            cout<<"\n";
        } 
        else
        {
            char *sendmsg = (char*)s.c_str();
            send(sock , sendmsg , s.size() , 0 ); 

            valread = recv( sock , buffer, MSGSIZE, 0);

            buffer[valread] = '\0' ;
            string msg_recv = string(buffer);
            cout<<msg_recv<<"\n";
        }
        

    }while(1);

    close(newconn); 
          
    return 0; 
} 
