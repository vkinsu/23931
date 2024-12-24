#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/select.h>

#define SOCKET_PATH "/tmp/my_socket"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS  FD_SETSIZE

int main() {
    int server_fd, client_fd, max_fd, i;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    fd_set readfds, allfds;

    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&allfds);
    FD_SET(server_fd, &allfds);
    max_fd = server_fd;

    for (;;) {
        readfds = allfds;
        if (select(max_fd + 1, &readfds, NULL, NULL, NULL) == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(server_fd, &readfds)) {
            if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                perror("accept");
            } else {
                FD_SET(client_fd, &allfds);
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }
            }
        }

        for (i = server_fd + 1; i <= max_fd; i++) {
            if (i == server_fd) continue;
            if (FD_ISSET(i, &readfds)) {
                bytes_read = read(i, buffer, sizeof(buffer));
                if (bytes_read > 0) {
                    for (ssize_t j = 0; j < bytes_read; j++) {
                        buffer[j] = (char)toupper((unsigned char)buffer[j]);
                    }
                    fwrite(buffer, 1, bytes_read, stdout);
                    fflush(stdout);
                } else {
                    if (bytes_read == 0 || (bytes_read == -1 && errno != EAGAIN)) {
                        close(i);
                        FD_CLR(i, &allfds);
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
