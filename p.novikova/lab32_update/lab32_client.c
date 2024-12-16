#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>

#define SOCKET_PATH "./unix_domain_socket"

void client_with_delay() {
  int client_fd;
  struct sockaddr_un address;
  char buffer[1024];

  // Создаем сокет
  client_fd = socket(AF_UNIX, SOCK_STREAM, 0); // потоковый сокет
  if (client_fd == -1) {
    perror("\nSocket making error\n");
    exit(1);
  }

  // Настраиваем адрес сокета
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  // Устанавливаем соединение с сервером
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("\nConnection error\n");
    close(client_fd);
    exit(1);
  }

  const char *message = "Delayed message\n";

  while (1) {
    // Отправляем текст серверу
    if (write(client_fd, message, strlen(message)) == -1) {
      perror("\nWriting error");
      close(client_fd);
      exit(1);
    }

    sleep(5); // Задержка в 5 секунд
  }

  // Закрываем сокет
  close(client_fd);
}

void regular_client() {
  int client_fd;
  struct sockaddr_un address;
  char buffer[1024];

  // Создаем сокет
  client_fd = socket(AF_UNIX, SOCK_STREAM, 0); // потоковый сокет
  if (client_fd == -1) {
    perror("\nSocket making error\n");
    exit(1);
  }

  // Настраиваем адрес сокета
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  // Устанавливаем соединение с сервером
  if (connect(client_fd, (struct sockaddr *)&address, sizeof(address)) == -1) {
    perror("\nConnection error\n");
    close(client_fd);
    exit(1);
  }

  while (fgets(buffer, sizeof(buffer), stdin)) {
    // Отправляем текст серверу
    if (write(client_fd, buffer, strlen(buffer)) == -1) {
      perror("\nWriting error");
      close(client_fd);
      exit(1);
    }
  }

  // Закрываем сокет
  close(client_fd);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Usage: %s <delay|regular>\n", argv[0]);
    exit(1);
  }

  if (strcmp(argv[1], "delay") == 0) {
    client_with_delay();
  } else if (strcmp(argv[1], "regular") == 0) {
    regular_client();
  } else {
    printf("Invalid argument. Use 'delay' or 'regular'.\n");
    exit(1);
  }

  return 0;
}