#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>

#define SOCKET_PATH "./socket"
#define BUFFER_SIZE 256
#define MAX_EVENTS 10

void to_uppercase(char *str) {
    while (*str) {
        *str = toupper((unsigned char)*str);
        str++;
    }
}

int main() {
    int server_fd, client_fd, epoll_fd;
    struct sockaddr_un addr;
    char buffer[BUFFER_SIZE];

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

    // Установка сокета в неблокирующий режим
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    // Создание epoll-инстанса
    epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1");
        exit(EXIT_FAILURE);
    }

    // Добавление серверного сокета в epoll
    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &event) == -1) {
        perror("epoll_ctl: server_fd");
        exit(EXIT_FAILURE);
    }

    printf("Сервер запущен, ожидает подключения клиентов...\n");

    while (1) {
        struct epoll_event events[MAX_EVENTS];

        int num_events = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        for (int i = 0; i < num_events; i++) {
            if (events[i].data.fd == server_fd) {
                // Обработка нового подключения
                client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) {
                    perror("accept");
                    continue;
                }

                // Установка клиентского сокета в неблокирующий режим
                fcntl(client_fd, F_SETFL, O_NONBLOCK);

                // Добавление клиентского сокета в epoll
                event.events = EPOLLIN;
                event.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &event) == -1) {
                    perror("epoll_ctl: client_fd");
                    exit(EXIT_FAILURE);
                }

                printf("Клиент подключен.\n");
            } else {
                // Обработка входящих сообщений от клиента
                client_fd = events[i].data.fd;
                ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);
                if (bytes_read <= 0) {
                    // Ошибка или клиент отключился
                    close(client_fd);
                    printf("Клиент отключен.\n");
                } else {
                    buffer[bytes_read] = '\0'; // Завершение строки
                    to_uppercase(buffer);
                    printf("Получено от клиента: %s", buffer);
                    
                    // Отправляем обратно клиенту
                    write(client_fd, buffer, bytes_read);
                }
            }
        }
    }

    // Очистка
    close(server_fd);
    unlink(SOCKET_PATH);
    return 0;
}

