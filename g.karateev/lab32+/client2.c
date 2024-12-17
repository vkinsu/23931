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

    printf("Connected to server. Enter messages:\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        printf("Your message: ");
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            printf("\nClient closed.\n");
            break;
        }

        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // Отправляем сообщение на сервер
        if (write(sock_fd, buffer, strlen(buffer)) == -1) {
            perror("Writing error");
            break;
        }

        // Читаем ответ от сервера
        ssize_t bytes_read = read(sock_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Server responded: %s\n", buffer);
        } else {
            perror("Reading error");
            break;
        }
    }

    close(sock_fd);
    return 0;
}
