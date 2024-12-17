#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <sys/un.h>
#include <errno.h>

#define BUF_SIZE 1024

int main(int argc, char* argv[]) {
    struct sockaddr_un addr;
    int client;
    char buf[BUF_SIZE];


    if ((client = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket creating error");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    const char* socket_path = "socket";
    strncpy(addr.sun_path, socket_path, sizeof(addr.sun_path) - 1);

    if (connect(client, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("Connection error");
        close(client);
        exit(EXIT_FAILURE);
    }
    ssize_t bytes_read;
    while ((bytes_read = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        ssize_t bytes_written = 0;
        while (bytes_written < bytes_read) {
            ssize_t bytereceiv = write(client, buf + bytes_written, bytes_read - bytes_written);
            if (bytereceiv == -1) {
                perror("Write error");
                close(client);
                exit(EXIT_FAILURE);
            }
            bytes_written += bytereceiv;
        }
    }

    close(client);
    return 0;
}
