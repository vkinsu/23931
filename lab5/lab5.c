#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LINES 500
#define TIMEOUT 5  // Время ожидания в секундах

int fd;
char *file_data;
long lines_pos[MAX_LINES];
int line_ln[MAX_LINES];
int i;

void print_file_content() {
    printf("%s", file_data);
    munmap(file_data, i);
    close(fd);
    exit(0);
}

void alarm_handler(int signum) {
    print_file_content();
}

int main(int argc, char *argv[]) {
    int j = 0, line_number;
    struct stat file_stat;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <файл>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Error in opening file");
        exit(1);
    }

    if (fstat(fd, &file_stat) == -1) {
        perror("Error recieving information");
        close(fd);
        exit(1);
    }

    file_data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Error transfering to memory");
        close(fd);
        exit(1);
    }

    lines_pos[1] = 0L;
    for (i = 0; i < file_stat.st_size; i++) {
        if (file_data[i] == '\n') {
            j++;
            line_ln[i++] = j;
            lines_pos[i] = i + 1;
            j = 0;
        } else {
            j++;
        }
    }

    if (i >= MAX_LINES) {
        fprintf(stderr, "File contains more than %d lines\n", MAX_LINES - 1);
        munmap(file_data, file_stat.st_size);
        close(fd);
        exit(1);
    }

    // Вывод таблицы отступов и длин строк для проверки
    //for (int k = 1; k < i; k++) {
    //    printf("Строка %d: offset=%ld, length=%d\n", k, lines_pos[k], line_ln[k]);
    //}

    // Настройка обработчика сигнала SIGALRM
    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    while (1) {
        printf("Enter number of line (0 for exit): ");
        alarm(TIMEOUT);  // Установка таймера на 5 секунд
        if (scanf("%d", &line_number) == 1) {
            alarm(0);  // Отмена таймера, если ввод успешен
            if (line_number == 0) {
                break;
            }

            if (line_number < 1 || line_number >= i) {
                printf("Incorrect line number. You can choose from: 1-%d\n", i - 1);
                continue;
            }

            printf("Line %d: %.*s\n", line_number, line_ln[line_number], file_data + lines_pos[line_number]);
        } else {
            // Очистка буфера ввода
            while (getchar() != '\n');
            printf("Incorrect input. Please, enter number.\n");
            continue;
        }
    }

    munmap(file_data, file_stat.st_size);
    close(fd);
    return 0;
}
