#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define BUFFER_SIZE 1024

int main() {
    int pipefd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];
    char *text = "Hello, World! This is a test message with mixed case letters.";

    // Создаем канал
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Создаем дочерний процесс
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Дочерний процесс
        close(pipefd[1]); // Закрываем запись в канал

        // Читаем из канала
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, BUFFER_SIZE)) > 0) {
            for (ssize_t i = 0; i < bytes_read; i++) {
                buffer[i] = toupper(buffer[i]);
            }
            write(STDOUT_FILENO, buffer, bytes_read);
        }

        close(pipefd[0]); // Закрываем чтение из канала
        exit(EXIT_SUCCESS);
    } else {
        // Родительский процесс
        close(pipefd[0]); // Закрываем чтение из канала

        // Пишем в канал
        write(pipefd[1], text, strlen(text));

        close(pipefd[1]); // Закрываем запись в канал
        wait(NULL); // Ожидаем завершения дочернего процесса
    }

    return 0;
}
