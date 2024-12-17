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

    // Создание сокета
    if ((sock_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Настройка адреса
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Подключение к серверу
    if (connect(sock_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        perror("Connect error");
        close(sock_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connected to server. Sending automatic messages...\n");

    char buffer[BUFFER_SIZE];
    int counter = 0;

    // Основной цикл отправки сообщений
    while (1) {
        // Генерация автоматического сообщения с номером
        snprintf(buffer, BUFFER_SIZE, "AUTO_MESSAGE_%d", counter++);
        
        // Отправка сообщения на сервер
        if (write(sock_fd, buffer, strlen(buffer)) == -1) {
            perror("Write error");
            break;
        }

        // Чтение ответа от сервера
        ssize_t bytes_read = read(sock_fd, buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            printf("Server responded: %s\n", buffer);
        } else if (bytes_read == 0) {
            printf("Server closed the connection.\n");
            break;
        } else {
            perror("Read error");
            break;
        }

        sleep(2); // Задержка перед отправкой следующего сообщения
    }

    close(sock_fd);
    return 0;
}
