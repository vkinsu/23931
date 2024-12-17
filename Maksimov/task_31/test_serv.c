#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))

char *socket_path = "\0hidden";

int main(int argc, char *argv[]) {
    struct sockaddr_un addr;
    char buf[100];
    int server_fd, client_fd, rc, max_fd;
    int active_clients = 0;
    fd_set read_fds, active_fds;

    // Создаем сокет
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    if (*socket_path == '\0') {
        *addr.sun_path = '\0';
        strncpy(addr.sun_path+1, socket_path+1, sizeof(addr.sun_path)-2);
    } else {
        strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path)-1);
        unlink(socket_path);
    }

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) {
        perror("listen error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);
    max_fd = server_fd;

    while (1) {
        read_fds = active_fds; // копия для вызова select

        // Ждем событий
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select error");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // идем по всем дескрипторам
        for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == server_fd) {
                    
                    if ((client_fd = accept(server_fd, NULL, NULL)) == -1) {
                        perror("accept error");
                        continue;
                    }
                    FD_SET(client_fd, &active_fds);
                    active_clients++;
                    max_fd = MAX(client_fd, max_fd);
                } else {
                    
                    rc = read(fd, buf, sizeof(buf));
                    if (rc > 0) {
                        for (int i = 0; i < rc; i++) buf[i] = toupper(buf[i]);
                        printf("Client %d: %.*s\n", fd, rc, buf);
                    } else {
                        if (rc == 0) {
                            
                            printf("Client %d disconnected.\n", fd);
                        } else {
                            perror("read error");
                        }
                        close(fd);
                        FD_CLR(fd, &active_fds);
                        active_clients--;
                        if(!active_clients){
                            printf("All clients disconnected. Stopping server\n");
                            close(server_fd);
                            unlink(socket_path);
                            exit(0);
                        }
                    }
                }
            }
        }
    }

    close(server_fd);
    return 0;
}
