# P2P_file_sharing_system
- This is a group based file sharing system where users can share, download files from the group they belong to
TRACKER COMMANDS
g++ server.cpp -o tracker -lpthread
./tracker tracker_info.txt 1



CLIENT1 COMMANDS 
g++ client.cpp -o client -lpthread -lcrypto
./client 127.1.1.1:5757 tracker_info.txt

create_user beerus abc
login beerus abc
create_group group1
join_group group1
list_requests group1
accept_request group1 piyush		
list_groups
upload_file GATE.mp4 group1
upload_file lena.jpg group1
upload_file book.pdf group1	
upload_file song.mp4 group1
upload_file audio.mp3 group1
list_files group1
show_downloads
stop_share group1 book.pdf



CLIENT2 COMMANDS 
g++ client.cpp -o client -lpthread -lcrypto
./client 127.2.2.2:5858 tracker_info.txt

create_user piyush pqr
login piyush pqr
create_group group1
join_group group1
list_requests group1
leave_group group1
download_file group1 GATE.mp4 /downloads/
download_file group1 lena.jpg /downloads/
download_file group1 book.pdf /downloads/
download_file group1 song.mp4 /downloads/
download_file group1 audio.mp3 /downloads/
