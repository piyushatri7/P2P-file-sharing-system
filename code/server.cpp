#include <bits/stdc++.h> 
#include <unistd.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string> 
#include <cstring>
#include <arpa/inet.h>
#include <cstdlib>
#include <pthread.h>
using namespace std;
#define PORT 6969
#define MSGSIZE 512



typedef struct clientdata
{
    string uname;
    string pass;
    bool loggedin;
    int port;
    string ip;
    int sockfd;
}clientinfo;

typedef struct groupdata
{
    string gid;
    string owner;
    set<string> members;
    set<string> requests;
    set<string> files;
}groupinfo;

typedef struct filedata
{
    string fname;
    unordered_map<string,string> uname_path;
    long long fsize;
}fileinfo;

unordered_map<string , clientinfo*>     clientmap;          //uname->clientinfo struct
unordered_map<string , groupinfo*>      groupmap;           //groupid->groupinfo struct
unordered_map<string , fileinfo*>       filemap;            //filename->fileinfo struct
unordered_map<int , string>             identify_user;      //sockfd->uname

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = create_user​ <user_id> <passwd>
string create_user(char* cmd)
{   
    char *c = strtok(cmd, " ");                              //discard command name
                    
    char *p1 = (char*)strtok(NULL, " ");    //get username
    char *p2 = (char*)strtok(NULL, " ");     //get password 
     
    string uname = string(p1);
    string pass = string(p2);
    //cout<<"hello\n";
    //cout<<c<<" "<<uname<<"  "<<pass;
    
    string reply;

    if(clientmap.find(uname) == clientmap.end())
    {
        clientinfo *newuser = new clientinfo();
        newuser->uname = (uname);
        newuser->pass = (pass);
        newuser->loggedin = false;

        clientmap[(uname)] = newuser;     //add new user entry
        //clientmap[uname] = newuser;
        
        cout<<"user created, username-> "<<clientmap[uname]->uname<<"\n";
        reply = "user created\n";
    }
    else
    {
        cout<<"user already exists\n";
        reply = "user already exists\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = login ​ <user_id> <passwd>
string login(char* cmd,int cl_socket)
{
    char *c = strtok(cmd, " ");                              //discard command name          
    char *p1 = (char*)strtok(NULL, " ");    //get username
    char *p2 = (char*)strtok(NULL, " ");     //get password
    char *p3 = (char*)strtok(NULL, " ");     //get IP address 
    char *p4 = (char*)strtok(NULL, " ");     //get Port number as string  
     
    string uname = string(p1);
    string pass = string(p2);
    string listen_ip = string(p3);
    string listen_port = string(p4);
    //cout<<c<<" "<<uname<<"  "<<pass;
    
    string reply ;
    if(clientmap.find(uname) != clientmap.end() )
    {
        
        if((clientmap[uname]->pass) == pass)
        {   
            if(clientmap[uname]->loggedin)          //if user already logged in
            {
                cout<<"Already logged in\n";
                reply = "Already logged in\n";
            }
            else
            {
                clientmap[uname]->loggedin = true;              //logged in status update
                clientmap[uname]->ip = listen_ip;               //IP address update
                clientmap[uname]->port = atoi(listen_port.c_str());       //Port number update

                identify_user[cl_socket] = uname;      //map sockfd to username

                cout<<"Logged in as: "<<clientmap[(uname)]->uname<<"\n";
                cout<<"Login Status:"<<clientmap[(uname)]->loggedin<<"\n";
                cout<<"IP address: "<<(clientmap[uname]->ip)<<"\n";
                cout<<"Port Number: "<<(clientmap[uname]->port)<<"\n";
                reply = "logged in\n";
            }
        }
        else
        {
           cout<<"Incorrect password\n";
           reply = "Incorrect password\n";
       }
    }
    else
    {
        cout<<"User doesn't Exist\n";
        reply = "User doesn't Exist\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = create_group​ <group_id>
string create_group(char* cmd,int cl_socket)
{   
    string reply;
    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {
        cout<<"login first please\n";
        reply = "login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];
    

    if(clientmap[uname]->loggedin)          //if user is logged in
    {
        char *c = strtok(cmd, " ");              //discard command name                    
        char *p1 = (char*)strtok(NULL, " ");    //get gorup id         
        string gid = string(p1);

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {   
            groupinfo *newgrp = new groupinfo();

            newgrp->gid = gid;
            newgrp->owner = uname;
            (newgrp->members).insert(uname);

            cout<<newgrp->gid<<" "<<newgrp->owner<<"\n";

            groupmap[gid] = newgrp;

            cout<<"group created: "<< groupmap[gid]->gid <<"\nowner: "<<groupmap[gid]->owner<<"\n";
            reply = "group created, You are the owner\n";
        }
        else
        {
            cout<<"group exists already\n";
            reply = "group exists already\n";
        }
    }
    else
    {
        cout<<"Please login to create user\n";
        reply= "Please login to create user\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = join_group​ <group_id>
string join_group(char *cmd, int cl_socket)
{
    string reply;

    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];     
    
    if(clientmap[uname]->loggedin)              //if user is logged in
    {
        char *c = strtok(cmd, " ");              //discard command name                    
        char *p1 = (char*)strtok(NULL, " ");    //get gorup id         
        string gid = string(p1);

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"group doesn't exist\n";
            reply="group doesn't exist\n";
        }
        else if((groupmap[gid]->requests).find(uname) != (groupmap[gid]->requests).end())  //request already sent
        {   
            cout<<"request already sent\n";
            reply="request already sent\n";
        }
        else if((groupmap[gid]->members).find(uname) != (groupmap[gid]->members).end())  //user already present
        {   
            cout<<"user already present\n";
            reply="user already present\n";
        }
        else                                               //new join request 
        {
            (groupmap[gid]->requests).insert(uname);      //push request to 

            cout<<"Group join request sent by "<<uname<<" to "<<gid<<"\n";
            reply="Group join request sent\n";
        }
    }
    else
    {
        cout<<"Please login to join group\n";
        reply="Please login to join group\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = list_requests <group_id> 
string list_requests(char *cmd, int cl_socket)
{
    string reply;

    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {   
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];     

    if(clientmap[uname]->loggedin)              //if user is logged in
    {
        char *c = strtok(cmd, " ");              //discard command name                    
        char *p1 = (char*)strtok(NULL, " ");    //get gorup id         
        string gid = string(p1);

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"group doesn't exist\n";
            reply="group doesn't exist\n";
        }
        /*
        else if(groupmap[gid]->owner != uname)              //if list is requested by someone other than owner
        {
            cout<<"You are not the owner, cannot show requests\n";
            reply="You are not the owner, cannot show requests\n";
        }
        */
        else    
        {
            reply = "";
            for(auto req : groupmap[gid]->requests)
                    reply += (req + " | ");

            cout<<"request list -> "<<reply<<"\n";
            reply="request list -> " + reply + "\n";
        }
    }
    else
    {
        cout<<"Please login to see request list\n";
        reply="Please login to see request list\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = accept_request <group_id> <user_id>
string accept_request(char *cmd, int cl_socket)
{
    string reply;

    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {   
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }
    //cout<<"debug\n";
    string uname = identify_user[cl_socket];        //client who issued command

    if(clientmap[uname]->loggedin)                  //if user is logged in
    {
        char *c = strtok(cmd, " ");             //discard command name
                    
        char *p1 = (char*)strtok(NULL, " ");    //get group id
        char *p2 = (char*)strtok(NULL, " ");    //get user id 
         
        string req_gid = string(p1);
        string req_uid = string(p2);


        if(groupmap.find(req_gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"group doesn't exist\n";
            reply="group doesn't exist\n";
        }
        
        else if(groupmap[req_gid]->owner != uname)              //if list is requested by someone other than owner
        {
            cout<<"You are not authorized to accept requests\n";
            reply="You are not authorized to accept requests\n";
        }
        else if((groupmap[req_gid]->requests).find(req_uid) == (groupmap[req_gid]->requests).end()) //request not found
        {
            cout<<"request not found\n";
            reply="request not found\n";
        }
        else    
        {
            (groupmap[req_gid]->members).insert(req_uid);      //accept request 
            (groupmap[req_gid]->requests).erase(req_uid);      //accept request 

            cout<<"Join request accepted for user "<<req_uid<<"\n";
            reply="Join request accepted";
        }
    }
    else
    {
        cout<<"Please login to accept request\n";
        reply="Please login to accept request\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = list_groups 
string list_groups(int cl_socket)
{
    string reply;

    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {   
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }
    //cout<<"debug\n";
    string uname = identify_user[cl_socket];        //client who issued command

    if(clientmap[uname]->loggedin)                  //if user is logged in
    {
        reply="";
        for (auto x : groupmap) 
            reply += ((x.second)->gid + " | "); 

        cout<<"group list -> "<< reply <<"\n";
        reply="group list -> " + reply + "\n";
    }    
    else
    {
        cout<<"Please login to see group list\n";
        reply="Please login to see group list\n";
    }
    return reply;  
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = leave_group <group_id> 
string leave_group(char *cmd,int cl_socket)
{
    string reply;

    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {   
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];        //client who issued command

    if(clientmap[uname]->loggedin)              //if user is logged in
    {
        char *c = strtok(cmd, " ");              //discard command name                    
        char *p1 = (char*)strtok(NULL, " ");    //get gorup id         
        string gid = string(p1);

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"group doesn't exist\n";
            reply="group doesn't exist\n";
        }
        else if((groupmap[gid]->members).find(uname) == (groupmap[gid]->members).end())  //user not present in the group
        {   
            cout<<"You are not a member\n";
            reply="You are not a member\n";
        }   
        else if((groupmap[gid]->owner) == uname)            //if owner wants to leave group
        {
            cout<<"Owner cannot leave the group\n";
            reply="Owner cannot leave the group\n";
        }
        else                                                //remove user 
        {
            (groupmap[gid]->members).erase(uname);          //erase user from group 

            cout<<"Group left by user: "<<uname<<" group id: "<<gid<<"\n";
            reply="Group left\n";
        }
    }
    else
    {
        cout<<"Please login to leave group\n";
        reply="Please login to leave group\n";
    }
    return reply;

}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = logout 
string logout(int cl_socket)
{
    string reply;

    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {   
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];        //client who issued command

    if(clientmap[uname]->loggedin)                  //if user is logged in
    {
        clientmap[uname]->loggedin = 0;

        cout<<"logged out user:"<<uname<<"\n"; 
        reply="logged out\n"; 
    }
    else
    {
        cout<<"You are not logged in\n"; 
        reply="You are not logged in\n"; 
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = upload_file <file_path> <group_id> 
string upload_file(char* cmd,int cl_socket)
{
    string reply;
    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];

    if(clientmap[uname]->loggedin)          //if user is logged in
    {
        string c = string(cmd);
        stringstream tokenize(c);
        string x,fpath,gid,fsize;

        //case when filename has space NOT handled

        getline(tokenize, x, ' ');              //get command name for processing
        getline(tokenize, fpath, ' ');          //get file path
        getline(tokenize, gid, ' ');            //get group id
        getline(tokenize, fsize, ' ');          //get file size
        long long file_size = stoi(fsize);

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"Group doesn't exist\n";
            reply="Group doesn't exist\n";
        }
        else if((groupmap[gid]->members).find(uname) == (groupmap[gid]->members).end())
        {
            cout<<"Please join the group first\n";
            reply="Please join the group first\n";
        }
        else
        {
            stringstream tokenize(fpath);
            string file_name;
            while(getline(tokenize , file_name, '/')) 
                ;
            
            (groupmap[gid]->files).insert(file_name);

            fileinfo *newfile = new fileinfo();
            newfile->fname = file_name;
            newfile->uname_path[uname] = fpath;
            newfile->fsize = file_size;

            filemap[file_name] = newfile;
            
            cout<<"file uploaded: "<<filemap[file_name]->fname
                <<"\nfilepath: "<<filemap[file_name]->uname_path[uname]
                <<"\nuploaded to group: "<<gid
                <<"\nfile size: "<<filemap[file_name]->fsize<<"\n";

            reply="file uploaded\n";

        } 
    }
    else
    {
        cout<<"Please login to upload file\n";
        reply="Please login to upload file\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = list_files <group_id>
//user need not be member to list files
string list_files(char* cmd,int cl_socket)
{
    string reply;
    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];

    if(clientmap[uname]->loggedin)          //if user is logged in
    {
        string c = string(cmd);
        stringstream tokenize(c);
        string x,fpath,gid;

        getline(tokenize, x, ' ');              //get command name for processing
        getline(tokenize, gid, ' ');            //get group id

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"Group doesn't exist\n";
            reply="Group doesn't exist\n";
        }
        else
        {
            reply = "";
            for(auto file : groupmap[gid]->files)
                    reply += (file + " | ");

            cout<<"File list: "<<reply;
            reply="File list: " +reply;
        }
    }
    else
    {
        cout<<"Please login to list file\n";
        reply="Please login to list file\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = downlaod_file <group_id> <file_name> <destination_path>
string download_file(char* cmd,int cl_socket)
{
    string reply;
    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];

    if(clientmap[uname]->loggedin)          //if user is logged in
    {
        string c = string(cmd);
        stringstream tokenize(c);
        string x,fname,gid,destpath;

        //case when filename has space NOT handled

        getline(tokenize, x, ' ');              //get command name for processing
        getline(tokenize, gid, ' ');            //get group id
        getline(tokenize, fname, ' ');          //get file name
        getline(tokenize, destpath, ' ');       //get destination path


        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"Group doesn't exist\n";
            reply="Group doesn't exist\n";
        }
        else if((groupmap[gid]->members).find(uname) == (groupmap[gid]->members).end())
        {
            cout<<"Please join the group first\n";
            reply="Please join the group first\n";
        }
        else if(filemap.find(fname) == filemap.end())
        {
            cout<<"File doesn't exist\n";
            reply="File doesn't exist\n";
        }
        else
        {
            string file_owner = (filemap[fname]->uname_path).begin()->first;       //one of the owner of file
            int owner_port = clientmap[file_owner]->port;
            string owner_ip = clientmap[file_owner]->ip;
            string file_path = filemap[fname]->uname_path[file_owner];

            cout<<file_path<<"\n";
            //cout<<"seeder ip and port: "<<owner_ip<<" "<<owner_port<<"\n";
            string peer_online_status;
            if(clientmap[file_owner]->loggedin)
                peer_online_status = "online";
            else
                peer_online_status = "offline";

            reply = peer_online_status + " " + to_string(owner_port) + " " + owner_ip + " " + file_path ;
            cout<<"sending seed data to peer: "<<reply<<"\n";
        }
    }
    else
    {
        cout<<"Please login to upload file\n";
        reply="Please login to upload file\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//cmd = stop_share <group_id> <file_name>
string stop_share(char* cmd,int cl_socket)
{
    string reply;
    if(identify_user.find(cl_socket) == identify_user.end())        //if user disconnected/not found
    {
        cout<<"login first please\n";
        reply="login first please\n";
        return reply;
    }

    string uname = identify_user[cl_socket];

    if(clientmap[uname]->loggedin)          //if user is logged in
    {
        string c = string(cmd);
        stringstream tokenize(c);
        string x,fname,gid,destpath;

        //case when filename has space NOT handled

        getline(tokenize, x, ' ');              //get command name for processing
        getline(tokenize, gid, ' ');            //get group id
        getline(tokenize, fname, ' ');          //get file name

        if(groupmap.find(gid) == groupmap.end())            //group doesn't exist
        {
            cout<<"Group doesn't exist\n";
            reply="Group doesn't exist\n";
        }
        else if(filemap.find(fname) == filemap.end())       //file doesn't exist
        {
            cout<<"File doesn't exist\n";
            reply="File doesn't exist\n";
        }
        else
        {
            filemap.erase(fname);
            (groupmap[gid]->files).erase(fname);
            cout<<"stopped sharing file: "<<fname<<"\n";
            reply="stopped sharing file: " + fname + "\n";
        }

    }
    else
    {
        cout<<"Please login to upload file\n";
        reply="Please login to upload file\n";
    }
    return reply;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void * handleConn(void *param)
{
    int cl_socket = *(int*)param;
    free(param);

    while(1)
    {    
        char message[MSGSIZE] = {0};                          
        int valread = read( cl_socket , message , MSGSIZE );
        
        if(valread <= 0 )        //client disconnects
            break;
        message[valread] = '\0';
        //cout<<message<<" ";
        //cout<<valread<<"\n";
        //cout<<"debug\n";
        string temp = message;

        char *cmd = strtok(message, " ");

        string command = cmd;

        char *x = (char *) temp.c_str();
        string reply ;


        if(command == "create_user")
        {      
            reply = create_user(x);
        }
        else if(command == "login")
        {
            reply = login(x,cl_socket);
        }
        else if(command == "create_group")
        {
            reply = create_group(x,cl_socket);
        }
        else if(command == "logout")
        {
            reply = logout(cl_socket);
        }
        else if(command == "join_group")
        {
            reply = join_group(x,cl_socket);
        }
        else if(command == "list_requests")
        {
            reply = list_requests(x,cl_socket);
        }
        else if(command == "accept_request")
        {
            reply = accept_request(x,cl_socket);
        }
        else if(command == "list_groups")
        {
            reply = list_groups(cl_socket);
        }
        else if(command == "leave_group")
        {
            reply = leave_group(x,cl_socket);
        }
        else if(command == "logout")
        {
            reply = logout(cl_socket);
        }
        else if(command == "upload_file")
        {
            reply = upload_file(x,cl_socket);
        }
        else if(command == "list_files")
        {
            reply = list_files(x,cl_socket);
        }
        else if (command == "download_file")
        {
            reply = download_file(x,cl_socket);
        }
        else if (command == "stop_share")
        {
            reply = stop_share(x,cl_socket);
        }
        else
        {
            cout<<"invalid command :"<<command<<":\n";
            reply="invalid command\n";
        }
        //printf("msg receieved: %s\n",message );
        cout<<"\n";
        
        char* replymsg = (char*)reply.c_str();
        unsigned int sent_size = send(cl_socket , replymsg , strlen(replymsg) , 0 ); 

    }

    if(identify_user.find(cl_socket) != identify_user.end())
    {
        string disc_uname = identify_user[cl_socket];   
        clientmap[disc_uname]->loggedin = 0;            //logout if user disconnected
    }

    cout<<"\nclient disconnected\n";
    close(cl_socket);

    return NULL;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int main(int argc, char const *argv[]) 
{ 
    int server_fd, new_socket; 
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[MSGSIZE] = {0}; 
       
    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    }   
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 

    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY;   //0.0.0.0 listen to all interfaces on our local machine
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 6969 
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
            cout<<"new client connected\n"<<inet_ntoa(address.sin_addr)<<" "<<address.sin_port<<"\n";
        } 
        

        pthread_t th;

        int *clsocket = (int*)malloc(sizeof(int));
        *clsocket = new_socket;

        int rc = pthread_create(&th, NULL, handleConn, (void*)clsocket);
        if (rc)
        {
         printf("ERROR; return code from pthread_create() is %d\n", rc);
         exit(-1);
        }


        //pthread_join(th, NULL); 
        //pthread_exit(NULL); 
    }
    return 0; 
} 