all: client server

client: task_31_client.c
	gcc -o client task_31_client.c -lsocket -lnsl

server: task_31_server.c
	gcc -o server task_31_server.c -lsocket -lnsl

clear:
	rm client 
	rm server
	rm /tmp/socket
