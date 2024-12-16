all: client server clientC

client: task_32_client.c
	gcc -o client task_32_client.c -lsocket -lnsl

server: task_32_server.c
	gcc -o server task_32_server.c -lsocket -lnsl

clientC: task_32_clientConst.c
	gcc -o clientc task_32_clientConst.c -lsocket -lnsl

clear:
	rm client 
	rm server
	rm /tmp/socket32
