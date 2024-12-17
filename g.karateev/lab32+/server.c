#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <ctype.h>
#include <pthread.h>

#define SOCKET_PATH "./unix_socket"
#define BUFFER_SIZE 1024
#define BACKLOG 5

int client1_fd = -1; // Первый клиент
int client2_fd = -1; // Второй клиент
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

// Обработка клиента
void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0'; // Завершаем строку

        pthread_mutex_lock(&clients_mutex);
        if (client_fd == client1_fd) {
            for (int i = 0; buffer[i] != '\0'; i++) {
                buffer[i] = toupper(buffer[i]); \
            }
            pthread_mutex_unlock(&clients_mutex);

            // Задержка перед отправкой ответа клиенту 1
            sleep(1);

            // Отправляем обработанное сообщение обратно клиенту 1
            if (write(client_fd, buffer, strlen(buffer)) == -1) {
                perror("Error sending to client 1");
            }
        } else if (client_fd == client2_fd) {
            // Обработка сообщений второго клиента (ручной ввод)
            pthread_mutex_unlock(&clients_mutex);

            // Перенаправляем сообщение клиенту 1
            pthread_mutex_lock(&clients_mutex);
            if (client1_fd != -1) {
                if (write(client1_fd, buffer, strlen(buffer)) == -1) {
                    perror("Error forwarding to client 1");
                }
            } else {
                printf("Client 1 is not connected, message dropped: %s\n", buffer);
            }
            pthread_mutex_unlock(&clients_mutex);

            // Отправляем подтверждение второму клиенту
            const char *ack = "Message received\n";
            if (write(client_fd, ack, strlen(ack)) == -1) {
                perror("Error sending ACK to client 2");
            }
        }
    }

    pthread_mutex_lock(&clients_mutex);
    if (client_fd == client1_fd) {
        client1_fd = -1;
        printf("Client 1 disconnected.\n");
    } else if (client_fd == client2_fd) {
        client2_fd = -1;
        printf("Client 2 disconnected.\n");
    }
    pthread_mutex_unlock(&clients_mutex);

    close(client_fd);
    return NULL;
}

int main() {
    int server_fd, client_fd;
    struct sockaddr_un address;
    struct sockaddr_un client_address;
    socklen_t client_len;
    pthread_t client_thread;

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    unlink(SOCKET_PATH);

    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("Bind error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("Listen error");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server started and listening on %s\n", SOCKET_PATH);

    while (1) {
        client_len = sizeof(client_address);
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
        if (client_fd == -1) {
            perror("Accept error");
            continue;
        }

        pthread_mutex_lock(&clients_mutex);
        if (client1_fd == -1) {
            client1_fd = client_fd;
            printf("Client 1 connected.\n");
        } else if (client2_fd == -1) {
            client2_fd = client_fd;
            printf("Client 2 connected.\n");
        } else {
            printf("Maximum clients connected. Rejecting new client.\n");
            close(client_fd);
            pthread_mutex_unlock(&clients_mutex);
            continue;
        }
        pthread_mutex_unlock(&clients_mutex);

        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;
        if (pthread_create(&client_thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("Thread creation error");
            close(client_fd);
            free(client_fd_ptr);
        } else {
            pthread_detach(client_thread);
        }
    }

    close(server_fd);
    unlink(SOCKET_PATH);

    return 0;
}
