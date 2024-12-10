#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <errno.h>
#include <sys/select.h>

#define SOCKET_PATH "./unix_socket"
#define BUFFER_SIZE 1024
#define BACKLOG 5

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char) str[i]);
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds, active_fds;
    int active_clients = 0;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);
    max_fd = server_fd;

    printf("Server started and listening on: %s\n", SOCKET_PATH);

    while (1) {
        read_fds = active_fds;

        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        for (int i = 0; i <= max_fd; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == server_fd) {
                    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                        perror("accept");
                        continue;
                    }
                    FD_SET(client_fd, &active_fds);
                    if (client_fd > max_fd) {
                        max_fd = client_fd;
                    }
                    active_clients++;
                    printf("Client connected (total clients: %d)\n", active_clients);
                } else {
                    ssize_t bytes_read = read(i, buffer, sizeof(buffer) - 1);
                    if (bytes_read > 0) {
                        buffer[bytes_read] = '\0';
                        to_uppercase(buffer);
                        printf("Client %d: %s\n", i, buffer);
                    } else {
                        if (bytes_read == 0) {
                            printf("Client %d disconnected\n", i);
                        } else {
                            perror("read");
                        }
                        close(i);
                        FD_CLR(i, &active_fds);
                        active_clients--;
                        printf("Client disconnected (total clients: %d)\n", active_clients);

                        if (active_clients == 0) {
                            printf("No more clients. Server shutting down.\n");
                            close(server_fd);
                            unlink(SOCKET_PATH);
                            return 0;
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
