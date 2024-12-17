#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>



void to_uppercase(char* str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    fd_set active_fds, read_fds;
    struct sockaddr_un addr;
    int server_fd, client_fd, max_fd;
    char buff[2048];
    int clients_list[10] = { 0 };
    int addr_len = sizeof(addr);
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("ssocket creation failed");
        exit(EXIT_FAILURE);
    }

    unlink("socket");

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, "socket", sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) == -1) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    FD_ZERO(&active_fds);
    FD_SET(server_fd, &active_fds);
    max_fd = server_fd;


    while (1) {
        read_fds = active_fds;
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select failed");
            exit(EXIT_FAILURE);
        }
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, (struct sockaddr*)&addr, (socklen_t*)&addr_len)) == -1) {
                perror("accept failed");
                continue;
            }

            printf("client %d connected\n", client_fd);

            for (int i = 0; i < 10; i++) {
                if (clients_list[i] == 0) {
                    clients_list[i] = client_fd;
                    FD_SET(client_fd, &active_fds);
                    if (client_fd > max_fd) max_fd = client_fd;
                    break;
                }
            }
        }

        for (int i = 0; i < 10; i++) {
            int sock = clients_list[i];

            if (FD_ISSET(sock, &read_fds)) {
                int bytes_read = read(sock, buff, 2048 - 1);

                if (bytes_read <= 0) {
                    printf("client %d disconnected\n", sock);
                    close(sock);
                    FD_CLR(sock, &active_fds);
                    clients_list[i] = 0;
                }
                else {
                    buff[bytes_read] = '\0';
                    to_uppercase(buff);
                    printf("client %d: %s\n", sock, buff);
                }
            }
        }
    }

    close(server_fd);
    unlink("socket");
    return 0;
}
