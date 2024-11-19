CC = gcc
CFLAGS = -Wall -Wextra

all: server client

server: lab30_server.c
	$(CC) $(CFLAGS) -o server lab30_server.c

client: lab30_client.c
	$(CC) $(CFLAGS) -o client lab30_client.c

clean:
	rm -f server client