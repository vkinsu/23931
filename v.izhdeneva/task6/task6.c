#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<signal.h>

#define MAX_N 100
#define MAX_M 256

void handle_alarm(int sig) {
    printf("\nTime's up! Exiting...\n");

    int fd = open("input.txt", O_RDONLY);
    char buffer[MAX_M];
    ssize_t bytesRead;
    size_t i = 0;

    while ((bytesRead = read(fd, buffer + i, 1)) > 0) {
        if (buffer[i] == '\n') {
            buffer[i] = '\0'; // Завершаем строку
            printf("%s\n", buffer); // Печатаем строку
            i = 0; // Сбрасываем индекс
        } else {
            i++;
            if (i >= MAX_M - 1) {
                // Если размер буфера достигнут, выводим строку
                buffer[i] = '\0';
                printf("%s\n", buffer);
                i = 0; // Сбрасываем индекс
            }
        }
    }

    // Печатаем последнюю строку, если она не завершается новой строкой
    if (i > 0) {
        buffer[i] = '\0';
        printf("%s\n", buffer);
    }

    close(fd);
    _exit(0);
}

int main() {
    int fd = open("input.txt", O_RDONLY);

    if (fd == -1) {
        perror("Can't open file\n");
        return -1;
    }

    off_t offsets[MAX_N];
    int line_count = 0;

    char buffer[MAX_M];
    off_t current_offset = 0;
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, MAX_M)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (line_count == 0|| buffer[i] == '\n') {
                offsets[line_count++] = current_offset + i + 1;
            }
        }
        current_offset += bytesRead;
    }

    close(fd);
    offsets[0] = 0;
    line_count--;

    // for (int i = 0; i < line_count; i++) printf("String %d starts from byte %ld\n", i + 1, offsets[i]);
    
    fd = open("input.txt", O_RDONLY);

    if (fd == -1) {
        perror("Can't open file\n");
        return -1;
    }

    int string_num;

    while (1) {
        signal(SIGALRM, handle_alarm);
        alarm(5);

        printf("You have 5 seconds to enter the number of string! Number: ");
        int result = scanf("%d", &string_num);

        alarm(0);

        if (!result || (string_num < 1 || string_num > line_count)) {
            perror("Wrong input\n");
            close(fd);
            return -1;
        }

        if (string_num == 0) {
            return 0;
        }

        if (lseek(fd, offsets[string_num - 1], SEEK_SET) == (off_t)-1) {
            perror("Can't read string\n");
            close(fd);
            return -1;
        }

        ssize_t bytesRead = read(fd, buffer, MAX_M);

        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                buffer[i] = '\0'; 
                break;
            }
        }

        printf("String %d: %s\n", string_num, buffer); 
    }

    close(fd);

    return 0;
}
