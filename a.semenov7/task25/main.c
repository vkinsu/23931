#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int pipefd[2]; // Массив для хранения дескрипторов канала
    pid_t cpid;
    char buffer[BUFFER_SIZE];

    // Создание канала
    if (pipe(pipefd) == -1) {
        error("pipe");
    }

    // Создание дочернего процесса
    cpid = fork();
    if (cpid == -1) {
        error("fork");
    }

    if (cpid == 0) { // Дочерний процесс
        close(pipefd[1]); // Закрываем конец записи канала

        // Чтение из канала и преобразование в верхний регистр
        ssize_t bytes_read;
        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0'; // Завершаем строку
            for (int i = 0; i < bytes_read; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]); // Преобразуем в верхний регистр
            }
            printf("Полученный текст в верхнем регистре: %s", buffer);
        }

        if (bytes_read == -1) {
            error("read");
        }

        close(pipefd[0]); // Закрываем конец чтения канала
        exit(EXIT_SUCCESS);
    } else { // Родительский процесс
        close(pipefd[0]); // Закрываем конец чтения канала

        // Запись текста в канал
        const char *text = "Hello, World!\nThis is a test.\n";
        size_t text_length = strlen(text);
        ssize_t bytes_written = write(pipefd[1], text, text_length);

        if (bytes_written == -1) {
            error("write");
        }

        close(pipefd[1]); // Закрываем конец записи канала
        wait(NULL); // Ожидаем завершения дочернего процесса
        exit(EXIT_SUCCESS);
    }
}
