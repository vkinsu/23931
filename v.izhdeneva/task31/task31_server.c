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

    int client_socket = accept(server_socket, NULL, NULL);

    // check if we can't connect to client socket
    if (client_socket == -1) {
        perror("\nxxxx accept xxxx\n");
        exit(-1);
    }

    read(client_socket, buffer, sizeof(buffer));
    to_uppercase(buffer);
    printf("Client: %s\n", buffer);

    close(client_socket);
    close(server_socket);

    return 0;
}