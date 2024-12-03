all: client server

client: task30client.c
	gcc -o client task30client.c -lsocket -lnsl

server: task30server.c
	gcc -o server task30server.c -lsocket -lnsl

clear:
	rm client 
	rm server
