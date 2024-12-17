#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/epoll.h>
#include <ctype.h>
#include <errno.h>


#define MAX_EVENTS 10
#define BUFFER_SIZE 4096

#define SOCKET_PATH "socket"

void to_upper_case(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper(str[i]);
    }
}

int setup_server_socket() {
    struct sockaddr_un addr;
    int server_num;

    if ((server_num = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(struct sockaddr_un));
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    unlink(SOCKET_PATH);

    if (bind(server_num, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
        perror("bind");
        close(server_num);
        exit(EXIT_FAILURE);
    }

    if (listen(server_num, SOMAXCONN) == -1) {
        perror("listen");
        close(server_num);
        exit(EXIT_FAILURE);
    }

    return server_num;
}

int setup_epoll(int server_num) {
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        close(server_num);
        exit(EXIT_FAILURE);
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_num;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_num, &event) == -1) {
        perror("epoll_ctl: server_num");
        close(server_num);
        close(epoll_fd);
        exit(EXIT_FAILURE);
    }

    return epoll_fd;
}

void handle_new_client(int server_num, int epoll_fd) {
    int client_fd = accept(server_num, NULL, NULL);
    if (client_fd == -1) {
        perror("accept");
        return;
    }

    printf("new client connected (fd=%d)\n", client_fd);

    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
        perror("epoll_ctl: client_fd");
        close(client_fd);
    }
}

int handle_client_data(int client_fd, int epoll_fd) {
    char buffer[BUFFER_SIZE];
    ssize_t num_bytes = read(client_fd, buffer, sizeof(buffer) - 1);

    if (num_bytes == 0) {
        printf("client disconnected (fd=%d)\n", client_fd);
        close(client_fd);
        return 0;
    }
    else if (num_bytes > 0) {
        buffer[num_bytes] = '\0';
        printf("client %d: %s\n", client_fd, buffer);

        to_upper_case(buffer);

        char message_with_sender[BUFFER_SIZE];
        snprintf(message_with_sender, sizeof(message_with_sender), "from client %d: %s", client_fd, buffer);

        struct epoll_event events[MAX_EVENTS];
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, 0);
        for (int i = 0; i < num_fds; i++) {
            int target_fd = events[i].data.fd;
            if (target_fd != client_fd && target_fd != -1) { 
                if (write(target_fd, message_with_sender, strlen(message_with_sender)) == -1) {
                    perror("write");
                }
            }
        }
    }
    else if (num_bytes == -1 && errno != EAGAIN) {
        perror("read");
        close(client_fd);
    }

    return 1;
}

void run_server(int server_num, int epoll_fd) {
    struct epoll_event events[MAX_EVENTS];
    int running = 1;

    while (running) {
        int num_fds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (num_fds == -1) {
            perror("epoll_wait");
            break;
        }

        for (int i = 0; i < num_fds; i++) {
            int fd = events[i].data.fd;

            if (fd == server_num) {
                handle_new_client(server_num, epoll_fd);
            }
            else {
                handle_client_data(fd, epoll_fd);
            }
        }
    }
}

void cleanup(int server_num, int epoll_fd) {
    close(server_num);
    close(epoll_fd);
    unlink(SOCKET_PATH);
}

int main() {
    int server_num = setup_server_socket();
    int epoll_fd = setup_epoll(server_num);

    run_server(server_num, epoll_fd);

    cleanup(server_num, epoll_fd);
    return 0;
}
