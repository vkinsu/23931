#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "socket"
#define BUFFER_SIZE 2048

int main() {
    char buffer[BUFFER_SIZE];
    int client_num;
    struct sockaddr_un address_socket;


    if ((client_num = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&address_socket, 0, sizeof(address_socket));
    address_socket.sun_family = AF_UNIX;
    strncpy(address_socket.sun_path, SOCKET_PATH, sizeof(address_socket.sun_path) - 1);

    if (connect(client_num, (struct sockaddr*)&address_socket, sizeof(address_socket)) == -1) {
        perror("Connect failed");
        close(client_num);
        exit(EXIT_FAILURE);
    }

    while (1) {
        fgets(buffer, BUFFER_SIZE, stdin);

        buffer[strcspn(buffer, "")] = '\0';

        if (send(client_num, buffer, strlen(buffer), 0) == -1) {
            perror("Send failed");
            break;
        }

        if (strcmp(buffer, "exit") == 0) {
            break;
        }
    }

    close(client_num);
    return 0;
}
