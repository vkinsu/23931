#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./unix_socket"
#define BUFFER_SIZE 256

int main(){
    int server_fd, client_fd;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path)-1);

    if (bind(server_fd, (struct sockaddr_un*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Binding error!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("Listening error!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server launched. Waiting for Connections...\n");

    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
        perror("Accept Error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Client Connected\n");

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        ssize_t bytes_received = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_received <= 0) break;

        for (int i = 0; i < bytes_received; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        printf("Received and UPPED: %s\n", buffer);

    }

    printf("client disconnected, server shuting down...\n");

    close(client_fd);
    close(server_fd);

    unlink(SOCKET_PATH);
    return 0;
}