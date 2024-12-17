// client.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_socket"

int main() {
    int client_fd;
    struct sockaddr_un server_addr;
    char buffer[256];

    if ((client_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_UNIX;
    strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connect failed");
        close(client_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server. Enter text to send (Ctrl+D to exit):\n");

    // Считываем данные с ввода и отправляем серверу
    while (fgets(buffer, sizeof(buffer), stdin)) {
        if (write(client_fd, buffer, strlen(buffer)) == -1) {
            perror("Write failed");
            close(client_fd);
            exit(EXIT_FAILURE);
        }
    }

    close(client_fd);
    return 0;
}
