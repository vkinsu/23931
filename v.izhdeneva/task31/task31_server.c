#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./unix_domain_socket"
// #define PORT 8080

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    char buffer[1024] = {0};

    // create the server socket
    int server_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    // check if we can't create the socket
    if (server_socket == -1) {
        perror("\nxxxx socket xxxx\n");
        exit(-1);
    }
    unlink(SOCKET_PATH);
    // define the server address
    struct sockaddr_un server_address;
    server_address.sun_family = AF_UNIX;
    strncpy(server_address.sun_path, SOCKET_PATH, sizeof(server_address.sun_path) - 1);
    // server_address.sun_port = htons(PORT);
    // server_address.sun_addr.s_addr = INADDR_ANY;

    // bind the socket to our specified IP and port
    int bind_socket = bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));

    // check if we can't bind the socket
    if (bind_socket == -1) {
        perror("\nxxxx server bind xxxx\n");
        exit(-1);
    }

    // listen the socket
    int listen_socket = listen(server_socket, 5);

    // check if we can't listen the socket
    if (listen_socket == -1) {
        perror("\nxxxx listen xxxx\n");
        exit(-1);
    }

    fd_set read_sockets, master_sockets;
    int max_socket, new_socket;
    struct sockaddr_un client_address;
    socklen_t client_len;

    FD_ZERO(&master_sockets);
    FD_SET(server_socket, &master_sockets);
    max_socket = server_socket;

    while (1) {
        read_sockets = master_sockets;
        int select_socket = select(max_socket + 1, &read_sockets, NULL, NULL, NULL);
        if (select_socket == -1) {
	    perror("\nxxxx select xxxx\n");
	    exit(-1);
	}
	for (int i = 0; i <= max_socket; i++) {
	   if (FD_ISSET(i, &read_sockets)) {
	      if (i == server_socket) {
	         client_len = sizeof(client_address);
		 new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
		 if (new_socket == -1) {
		    perror("\nxxxx accept xxxx\n");
		 } else {
		    printf("new client connected\n");  
		    FD_SET(new_socket, &master_sockets);
		    if (new_socket > max_socket) {
		       max_socket = new_socket;
		    }
		 }
	      } else {
	      	 int bytes_received = read(i, buffer, sizeof(buffer));
		 if (bytes_received <= 0) {
		    perror("\nxxxx read xxxx\n");
		    close(i);
		    FD_CLR(i, &master_sockets);
		    exit(-1);
		 }
		 buffer[bytes_received] = '\0';
		 to_uppercase(buffer);
		 printf("CLient/%d: %s\n", i, buffer);
	      }
	   }
	}
    }

    close(server_socket);
    unlink(SOCKET_PATH);
    return 0;
}
 
