#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/un.h>
#include <fcntl.h>
#include <errno.h>
#include <ctype.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 256

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd, max_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];
    fd_set read_fds;
    
    // Создание сокета
    if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Удаление старого сокета, если он существует
    unlink(SOCKET_PATH);

    // Настройка адреса сокета
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    // Привязка сокета к адресу
    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Прослушивание входящих соединений
    if (listen(server_fd, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен, ожидает подключения клиентов...\n");

    // Массив для хранения дескрипторов клиентов
    int client_fds[10];
    int client_count = 0;

    while (1) {
        // Обнуление множества дескрипторов
        FD_ZERO(&read_fds);
        FD_SET(server_fd, &read_fds);
        max_fd = server_fd;

        // Добавление дескрипторов клиентов
        for (int i = 0; i < client_count; i++) {
            FD_SET(client_fds[i], &read_fds);
            if (client_fds[i] > max_fd) {
                max_fd = client_fds[i];
            }
        }

        // Ожидание активности на сокетах
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

        // Проверка нового подключения
        if (FD_ISSET(server_fd, &read_fds)) {
            if ((client_fd = accept(server_fd, NULL, NULL)) < 0) {
                perror("accept");
                continue;
            }

            // Добавление нового клиента в массив
            client_fds[client_count++] = client_fd;
            printf("Клиент подключен. Подключенных клиентов: %d\n", client_count);
        }

        // Обработка входящих сообщений от клиентов
        for (int i = 0; i < client_count; i++) {
            if (FD_ISSET(client_fds[i], &read_fds)) {
                ssize_t bytes_read = read(client_fds[i], buffer, sizeof(buffer) - 1);
                if (bytes_read <= 0) {
                    // Ошибка или клиент отключился
                    close(client_fds[i]);
                    client_fds[i] = client_fds[--client_count]; // Удаление клиента
                    printf("Клиент отключен. Подключенных клиентов: %d\n", client_count);
                } else {
                    buffer[bytes_read] = '\0'; // Завершение строки
                    to_uppercase(buffer);
                    printf("Получено от клиента: %s", buffer);
                }
            }
        }

        // Проверка на отсутствие подключенных клиентов
        if (client_count == 0) {
            printf("Нет подключенных клиентов. Сервер завершает работу.\n");
            break; // Выход из цикла, завершение работы сервера
        }
    }

    // Очистка
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

