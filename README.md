# Peer-to-Peer file sharing system

P2P is a file sharing technology, allowing the users to access mainly the multimedia files like videos, music, e-books, games etc. The individual users in this network are referred to as peers. The peers request for the files from other peers by establishing TCP or UDP connections. This is a group based file sharing system where users can share, download files from the group they belong to

### Architecture
- Tracker: The tracker is a special server that keeps track of the connected peers. Maintains information of clients with their files(shared by client) to assist the clients for the communication between peers.
- Peers/Client: Users downloading from a BitTorrent swarm are commonly referred to as “leechers” or “peers. 
- For downloading, one peer – who has a complete copy of all the files in the torrent – joins the network so other users can download the data.
- All data transfers are encryped and verified using SHA.

### Working
1. Client creates an account (userid and password) in order to be part of the network.
2. Client can create any number of groups(with different group ID) and hence will be owner of those groups.
3. Client needs to be part of the group from which it wants to download the file.
4. Client can also send join request to join a group. Owner Client will Accept/Reject the request.
5. After joining group ,client will be able to see list of all the shareable files in the group.
6. Client can share file in any group.(file will not get uploaded to tracker but only the <ip, port> of the client for that file)
7. Client sends the download command to tracker with the group name and filename and tracker will send the details of the group members which are currently sharing that particular file.
8. After fetching the peer info from the tracker, client will communicate with peers about the portions of the file they contain and hence accordingly decide which part of data to take from which peer.
9. Client can also stop download while downoad is going on.

### TRACKER COMMANDS


- ``` g++ server.cpp -o tracker -lpthread``` 
- Run Tracker: ``` ./tracker tracker_info.txt 1``` 


### CLIENT COMMANDS 

- ``` g++ client.cpp -o client -lpthread -lcrypto``` 
- Run Client: ``` ./client 127.1.1.1:5757 tracker_info.txt``` 
- Create User Account: ``` create_user user_name password``` 
- Login: ``` login user_name password``` 
- Create Group: ``` create_group group1``` 
- Join Group: ``` join_group group1``` 
- List pending join: ``` list_requests group1``` 
- Accept Group Joining Request: ``` accept_request group1 user_name	``` 
- List All Group In Network: ``` list_groups``` 
- Upload Files:
  - ``` upload_file video.mp4 group1``` 
  - ``` upload_file image.jpg group1``` 
  - ``` upload_file document.pdf group1	``` 
  - ``` upload_file song.mp4 group1``` 
  - ``` upload_file audio.mp3 group1``` 
- ``` list_files group1``` 
- Show ongoing downloads: ``` show_downloads``` 
- Stop sharing file: ``` stop_share group1 document.pdf``` 

