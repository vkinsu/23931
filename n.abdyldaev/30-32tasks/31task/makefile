all: client server

client: 31client.c
	gcc 31client.c -o client -lsocket

server: 31server.c
	gcc 31server.c -o server -std=c11 -lsocket

clean:
	rm -f server client socket

