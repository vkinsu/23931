#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>


#define SOCKET_PATH "./test"
#define BUFFER_SIZE 1024
#define MAX_EVENTS 10

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
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_un addr;
    struct epoll_event ev, events[MAX_EVENTS];
    char buffer[BUFFER_SIZE];

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

    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1!!");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    ev.events = EPOLLIN;
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) == -1) {
        perror("epoll_ctl!!");
        close(server_fd);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    printf("enter waiting %s\n", SOCKET_PATH);
    signal(SIGINT, handle_sigint);
    while (1) {
        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, 5000);
        if (num_events == -1) {
            perror("epoll_wait!!");
            break;
        }// else if (num_events == 0) {
         //   printf("Timeout: no events within 5 seconds.\n");
       // }

        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_fd) {
                client_fd = accept(server_fd, NULL, NULL);
                if (client_fd == -1) {
                    perror("accept!!");
                    continue;
                }
                printf("New client connected: %d\n", client_fd);

                ev.events = EPOLLIN;
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
                    perror("epoll_ctl: client_fd!!");
                    close(client_fd);
                }
            } else {
                int client_fd = events[i].data.fd;
                ssize_t len = read(client_fd, buffer, sizeof(buffer) - 1);

                if (len == -1) {
                    perror("read!!");
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                } else if (len == 0) {
                    printf("Client disconnected: %d\n", client_fd);
                    close(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                } else {
                    buffer[len] = '\0';
                    to_upper_case(buffer);
                    printf("Received from client %d: %s\n", client_fd,  buffer);
                    sleep(5);
		 }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    unlink(SOCKET_PATH);
    return 0;
}

