#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <ctype.h>
#include <pthread.h>

#define SOCKET_PATH "./unix_domain_socket"
// #define PORT 8080

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[1024] = {0};
    int read_message = read(client_socket, buffer, sizeof(buffer));
    if (read_message == -1) {
        perror("\nxxxx read xxxx\n");
	exit(-1);
    }
    to_uppercase(buffer);
    printf("received: %s\n", buffer);
    close(client_socket);
    free(arg);
    return NULL;
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

    int client_socket;
    struct sockaddr_un client_address;
    socklen_t client_len;
    pthread_t client_thread;

    while (1) {
        client_len = sizeof(client_address);
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        if (client_socket == -1) {
	    perror("\nxxxx accept xxxx\n");
	    continue;
        }
	int *client_socket_ptr = malloc(sizeof(int));
	*client_socket_ptr = client_socket;
	
	int pthread_creation = pthread_create(&client_thread, NULL, handle_client, client_socket_ptr); 
	if (pthread_creation != 0) {
	    perror("\nxxxx pthread create xxxx\n");
	    close(client_socket);
	    free(client_socket_ptr);
	} else {
	    pthread_detach(client_thread);
	}
    }
   
    close(server_socket);

    return 0;
}
