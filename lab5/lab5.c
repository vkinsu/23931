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
        fprintf(stderr, "Использование: %s <файл>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Ошибка открытия файла");
        exit(1);
    }

    if (fstat(fd, &file_stat) == -1) {
        perror("Ошибка получения информации о файле");
        close(fd);
        exit(1);
    }

    file_data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Ошибка отображения файла в память");
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
        fprintf(stderr, "Файл содержит более %d строк\n", MAX_LINES - 1);
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
        printf("Введите номер строки (0 для выхода): ");
        alarm(TIMEOUT);  // Установка таймера на 5 секунд
        if (scanf("%d", &line_number) == 1) {
            alarm(0);  // Отмена таймера, если ввод успешен
            if (line_number == 0) {
                break;
            }

            if (line_number < 1 || line_number >= i) {
                printf("Неверный номер строки. Доступные строки: 1-%d\n", i - 1);
                continue;
            }

            printf("Строка %d: %.*s\n", line_number, line_ln[line_number], file_data + lines_pos[line_number]);
        } else {
            // Очистка буфера ввода
            while (getchar() != '\n');
            printf("Неправильный ввод. Пожалуйста, введите число.\n");
            continue;
        }
    }

    munmap(file_data, file_stat.st_size);
    close(fd);
    return 0;
}
