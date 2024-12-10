#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <errno.h>

#define SOCKET_PATH "./unix_socket"
#define BUFFER_SIZE 1024
#define BACKLOG 5
#define MAX_CLIENTS 10

int server_fd;
int client_fds[MAX_CLIENTS] = {0};

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char) str[i]);
    }
}

void handle_sigio(int sig) {
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_fds[i] > 0) {
            ssize_t bytes_read = read(client_fds[i], buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                to_uppercase(buffer);
                printf("Client %d: %s\n", client_fds[i], buffer);
            } else if (bytes_read == 0) {
                printf("Client %d disconnected\n", client_fds[i]);
                close(client_fds[i]);
                client_fds[i] = 0;
            } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("read");
                close(client_fds[i]);
                client_fds[i] = 0;
            }
        }
    }
}

void add_client(int client_fd) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (client_fds[i] == 0) {
            client_fds[i] = client_fd;
            return;
        }
    }
    fprintf(stderr, "Too much clients!\n");
    close(client_fd);
}

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        exit(EXIT_FAILURE);
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl F_SETFL O_NONBLOCK");
        exit(EXIT_FAILURE);
    }
}

int main() {
    struct sockaddr_un addr;

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

    // Настраиваем серверный сокет для неблокирующего режима
    set_nonblocking(server_fd);

    if (fcntl(server_fd, F_SETOWN, getpid()) == -1) {
        perror("fcntl F_SETOWN");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    if (fcntl(server_fd, F_SETFL, O_NONBLOCK | O_ASYNC) == -1) {
        perror("fcntl F_SETFL O_ASYNC");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_sigio;
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server startes and listening on: %s\n", SOCKET_PATH);

    while (1) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) {
            if (errno != EAGAIN && errno != EWOULDBLOCK) {
                perror("accept");
            }
        } else {
            printf("Client connected: %d\n", client_fd);
            set_nonblocking(client_fd);
            if (fcntl(client_fd, F_SETOWN, getpid()) == -1) {
                perror("fcntl F_SETOWN");
            }
            if (fcntl(client_fd, F_SETFL, O_NONBLOCK | O_ASYNC) == -1) {
                perror("fcntl F_SETFL O_ASYNC");
            }
            add_client(client_fd);
        }

    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
