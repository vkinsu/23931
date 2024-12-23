#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 100
const char *default_socket_path = "./socket";

int main(int argc, char *argv[]) {
    struct sockaddr_un server_address;
    char buffer[BUFFER_SIZE];
    int server_fd, client_fd, bytes_read;

    const char *socket_path = (argc > 1) ? argv[1] : default_socket_path;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&server_address, 0, sizeof(server_address));
    server_address.sun_family = AF_UNIX;

    if (*socket_path == '0') {
        *server_address.sun_path = '0';
        strncpy(server_address.sun_path + 1, socket_path + 1, sizeof(server_address.sun_path) - 2);
    } else {
        strncpy(server_address.sun_path, socket_path, sizeof(server_address.sun_path) - 1);
        unlink(socket_path);
    }

    if (bind(server_fd, (struct sockaddr*)&server_address, sizeof(server_address)) == -1) {
        perror("Socket binding error");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1) == -1) {
        perror("Listening error");
        exit(EXIT_FAILURE);
    }

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            perror("Connection acceptance error");
            continue;
        }
        
        printf("Client connected.\n");

        while ((bytes_read = read(client_fd, buffer, sizeof(buffer))) > 0) {
            printf("Read %d bytes: ", bytes_read);
            for (int i = 0; i < bytes_read; i++) {
                printf("%c", toupper(buffer[i]));
            }
            printf("\n");
        }

        if (bytes_read == -1) {
            perror("Reading error");
            exit(EXIT_FAILURE);
        } else if (bytes_read == 0) {
            printf("Client disconnected.\n");
            close(client_fd);
            unlink(socket_path);
            break;
        }
    }

    return 0;
}
