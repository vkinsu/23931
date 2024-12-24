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
#include <signal.h>

#define SOCKET_PATH "/tmp/my_socket"
#define MAX_CLIENTS 1024
#define BUFFER_SIZE 1024

static volatile sig_atomic_t io_flag = 0;

static int clients[MAX_CLIENTS];
static int client_count = 0;
static int server_fd = -1;

static void set_nonblocking_async(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) flags = 0;
    fcntl(fd, F_SETFL, flags | O_NONBLOCK | O_ASYNC);
    fcntl(fd, F_SETOWN, getpid());
}

static void sigio_handler(int signo) {
    (void)signo;
    io_flag = 1;
}

static void handle_new_clients() {
    for (;;) {
        int cfd = accept(server_fd, NULL, NULL);
        if (cfd == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) break;
            perror("accept");
            break;
        }
        if (client_count < MAX_CLIENTS) {
            set_nonblocking_async(cfd);
            clients[client_count++] = cfd;
        } else {
            close(cfd);
        }
    }
}

static void handle_client_data() {
    char buffer[BUFFER_SIZE];
    for (int i = 0; i < client_count; ) {
        int cfd = clients[i];
        ssize_t n = read(cfd, buffer, sizeof(buffer));
        if (n > 0) {
            for (ssize_t j = 0; j < n; j++) {
                buffer[j] = (char)toupper((unsigned char)buffer[j]);
            }
            fwrite(buffer, 1, n, stdout);
            fflush(stdout);
            i++;
        } else if (n == 0) {
            close(cfd);
            clients[i] = clients[client_count - 1];
            client_count--;
        } else {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                i++;
            } else {
                perror("read");
                close(cfd);
                clients[i] = clients[client_count - 1];
                client_count--;
            }
        }
    }
}

int main() {
    struct sockaddr_un addr;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path)-1);

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

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

    set_nonblocking_async(server_fd);

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sigio_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGIO, &sa, NULL) == -1) {
        perror("sigaction");
        close(server_fd);
        unlink(SOCKET_PATH);
        exit(EXIT_FAILURE);
    }

    for (;;) {
        if (!io_flag) {
            pause(); 
        }
        io_flag = 0;
        handle_new_clients();
        handle_client_data();
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}
