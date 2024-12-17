#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>

#define BUF_SIZE 1024
#define SOCKET_PATH "socket"

int main(int argc, char* argv[]) {
    struct sockaddr_un addr;
    int client_socket;
    char buf[BUF_SIZE];

    if ((client_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(client_socket, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Connection error");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    while (fgets(buf, sizeof(buf), stdin) != NULL) {
        ssize_t bytes_written = 0;
        size_t bytes_read = strlen(buf);
        while (bytes_written < bytes_read) {
            ssize_t bytereceiv = write(client_socket, buf + bytes_written, bytes_read - bytes_written);
            if (bytereceiv == -1) {
                perror("Write error");
                close(client_socket);
                exit(EXIT_FAILURE);
            }
            bytes_written += bytereceiv;
        }
    }

    close(client_socket);
    return 0;
}

