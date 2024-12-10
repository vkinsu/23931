#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>

#define SOCKET_PATH "./unix_socket"
#define BUFFER_SIZE 1024

int main() {
    int sock_fd;
    struct sockaddr_un addr;

    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Sending data every 4 seconds...\n");

    char buffer[BUFFER_SIZE];
    int counter = 0;

    while (1) {
        snprintf(buffer, sizeof(buffer), "Message %d", counter++);
        
        if (write(sock_fd, buffer, strlen(buffer)) == -1) {
            perror("Writing error");
            break;
        }
        
        printf("Sent: %s\n", buffer);

        sleep(4);
    }

    close(sock_fd);
    return 0;
}
