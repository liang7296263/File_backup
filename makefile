all:client

client:client.cpp File_send.cpp ClientSocket.cpp ThreadPool.cpp inotify.cpp Backup.cpp
	g++ -g -o client client.cpp File_send.cpp ClientSocket.cpp ThreadPool.cpp inotify.cpp Backup.cpp -lpthread

clean:
	rm -f client
