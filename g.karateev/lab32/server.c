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
#define MAX_CLIENTS 10

int client_counter = 0;
int active_clients = 0; // Количество активных клиентов
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_fd = *(int *)arg;
    int client_id;
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Увеличиваем счетчик клиентов и присваиваем ID текущему клиенту
    pthread_mutex_lock(&counter_mutex);
    client_id = ++client_counter;
    active_clients++;
    pthread_mutex_unlock(&counter_mutex);

    printf("Client %d connected\n", client_id);

    while ((bytes_received = read(client_fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes_received] = '\0';

        // Преобразуем текст в верхний регистр
        for (int j = 0; buffer[j] != '\0'; j++) {
            buffer[j] = toupper(buffer[j]);
        }

        printf("Client %d: %s\n", client_id, buffer);
    }

    printf("Client %d disconnected\n", client_id);

    // Уменьшаем количество активных клиентов
    pthread_mutex_lock(&counter_mutex);
    active_clients--;
    int should_exit = (active_clients == 0); // Проверяем, нужно ли завершать сервер
    pthread_mutex_unlock(&counter_mutex);

    close(client_fd);
    free(arg);

    // Если активных клиентов нет, завершаем сервер
    if (should_exit) {
        printf("No more clients. Server shutting down.\n");
        exit(0);
    }

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
        perror("\nSocket creation error\n");
        exit(1);
    }

    unlink(SOCKET_PATH);

    address.sun_family = AF_UNIX;
    strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
        perror("\nServer bind error\n");
        close(server_fd);
        exit(1);
    }

    if (listen(server_fd, BACKLOG) == -1) {
        perror("\nServer listen error\n");
        close(server_fd);
        exit(1);
    }

    printf("Server started and listening on %s\n", SOCKET_PATH);

    while (1) {
        client_len = sizeof(client_address);
        client_fd = accept(server_fd, (struct sockaddr *)&client_address, &client_len);
        if (client_fd == -1) {
            perror("\nAccept error\n");
            continue;
        }

        int *client_fd_ptr = malloc(sizeof(int));
        *client_fd_ptr = client_fd;

        if (pthread_create(&client_thread, NULL, handle_client, client_fd_ptr) != 0) {
            perror("\nThread creation error\n");
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
