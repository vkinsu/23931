#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/select.h>
#include <ctype.h>
#include <signal.h>


#define SOCKET_PATH "./test"
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 10

void to_upper_case(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int server_fd;
void handle_sigint(int sig) {
    printf("\nShutting down server\n");
    close(server_fd);
    unlink(SOCKET_PATH);
    exit(0);
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un addr;
    fd_set read_fds;
    char buffer[BUFFER_SIZE];
    struct timeval timeout;
    int client_fds[MAX_CLIENTS];
    int i;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket!!");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind!!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen!!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    max_fd = server_fd;

    for (i = 0; i < MAX_CLIENTS; i++) {
        client_fds[i] = -1;
    }

    printf("Enter waiting\n", SOCKET_PATH);
    signal(SIGINT, handle_sigint);

    while (1) {
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        fd_set temp_fds = read_fds;

        int ready_fds = select(max_fd + 1, &temp_fds, NULL, NULL, &timeout);
        if (ready_fds == -1) {
            perror("select");
            break;
        }

        if (FD_ISSET(server_fd, &temp_fds)) {
            client_fd = accept(server_fd, NULL, NULL);
            if (client_fd == -1) {
                perror("accept");
                continue;
            }

            //  new socket
            for (i = 0; i < MAX_CLIENTS; i++) {
                if (client_fds[i] == -1) {
                    client_fds[i] = client_fd;
                    break;
                }
            }

            if (i == MAX_CLIENTS) {
                fprintf(stderr, "Max client connections reached. Rejecting new client.\n");
                close(client_fd);
            } else {
                FD_SET(client_fd, &read_fds);
                if (client_fd > max_fd) {
                    max_fd = client_fd;
                }

                printf("New client connected %d\n", client_fd);
            }
        }

        for (i = 0; i < MAX_CLIENTS; i++) {
            client_fd = client_fds[i];
            if (client_fd != -1 && FD_ISSET(client_fd, &temp_fds)) {
                ssize_t len = read(client_fd, buffer, sizeof(buffer) - 1);
                if (len == -1) {
                    perror("read");
                    close(client_fd);
                    FD_CLR(client_fd, &read_fds);
                    client_fds[i] = -1;
                } else if (len == 0) {
                    printf("Client disconnected %d\n", client_fd);
                    close(client_fd);
                    FD_CLR(client_fd, &read_fds);
                    client_fds[i] = -1;
                } else {
                    buffer[len] = '\0';
                    to_upper_case(buffer);
                    printf("Received from client: %s\n", buffer);
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}

