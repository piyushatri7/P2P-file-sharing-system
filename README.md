# Peer-to-Peer file sharing system
- This is a group based file sharing system where users can share, download files from the group they belong to

### Introduction
P2P is a file sharing technology, allowing the users to access mainly the multimedia files like videos, music, e-books, games etc. The individual users in this network are referred to as peers. The peers request for the files from other peers by establishing TCP or UDP connections. 

### Architecture
- Tracker: The tracker is a special server that keeps track of the connected peers. The tracker shares their IP addresses with clients in the network, allowing them to connect to each other.
- Peers: Users downloading from a BitTorrent swarm are commonly referred to as “leechers” or “peers. 
- For downloading, one peer – who has a complete copy of all the files in the torrent – joins the netword so other users can download the data.
- All data transfers are encryped and verified using SHA.

### TRACKER COMMANDS

- g++ server.cpp -o tracker -lpthread
- ./tracker tracker_info.txt 1



### CLIENT COMMANDS 

- g++ client.cpp -o client -lpthread -lcrypto
- ./client 127.1.1.1:5757 tracker_info.txt
- create_user user_name password
- login user_name password
- create_group group1
- join_group group1
- list_requests group1
- accept_request group1 user_name	
- list_groups
- upload_file video.mp4 group1
- upload_file image.jpg group1
- upload_file document.pdf group1	
- upload_file song.mp4 group1
- upload_file audio.mp3 group1
- list_files group1
- show_downloads
- stop_share group1 document.pdf
