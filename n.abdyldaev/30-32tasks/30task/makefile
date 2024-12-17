all: client server

client: 30client.c
	gcc 30client.c -o client -lsocket

server: 30server.c
	gcc 30server.c -o server -std=c11 -lsocket

clean:
	rm -f server client socket
