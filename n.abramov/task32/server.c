#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <ctype.h>
#include <signal.h>

#define BUFSIZE 1024
#define MAX_EVENTS 10
#define SOCKET_PATH "SOCKET"

int quit_flag = 0;

void handle_sigint(int sig) {
    quit_flag = 1;
}

void set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    struct sockaddr_un addr;
    int server_fd, epoll_fd;

    signal(SIGINT, handle_sigint);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    unlink(SOCKET_PATH);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
    listen(server_fd, SOMAXCONN);
    set_nonblocking(server_fd);

    epoll_fd = epoll_create1(0);

    struct epoll_event event, events[MAX_EVENTS];
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = server_fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event);

    while (!quit_flag) {
        int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < n; i++) {
            int current_fd = events[i].data.fd;

            if (current_fd == server_fd) {
                while (1) {
                    int client_fd = accept(server_fd, NULL, NULL); 
                    if (client_fd == -1) {
                        if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                    } else {
                        set_nonblocking(client_fd);
                        event.events = EPOLLIN | EPOLLET;
                        event.data.fd = client_fd;
                        epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event);
                    }
                }
            } else {
                char buf[BUFSIZE];
                while (1) {
                    memset(buf, 0, BUFSIZE);
                    ssize_t bytes_read = read(current_fd, buf, BUFSIZE - 1);

                    if (bytes_read > 0) {

                        for (ssize_t j = 0; j < bytes_read; j++) {
                            buf[j] = toupper((unsigned char)buf[j]);
                        }

                        printf("%s", buf);

                        ssize_t bytes_written = write(current_fd, buf, bytes_read);
                        if (bytes_written == -1) {
                            perror("write fail");
                        }
                    } else if (bytes_read == 0) {
                        printf("Client disconnected: %d\n", current_fd);
                        close(current_fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                        break;
                    } else if (errno == EAGAIN || errno == EWOULDBLOCK) {
                        
                        break;
                    } else {
                        perror("read fail");
                        close(current_fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, current_fd, NULL);
                        break;
                    }
                }
            }
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    close(epoll_fd);
    return 0;
}

