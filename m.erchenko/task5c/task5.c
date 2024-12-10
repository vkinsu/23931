#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

int main(int argc, char *argv[]) {
    int fd;
    char buffer[1024];
    ssize_t bytes_read;
    off_t position = 0;
    size_t line_count = 0;
    size_t array_size = 100;
    LineInfo *lines = malloc(array_size * sizeof(LineInfo));
    if (!lines) {
        perror("malloc");
        exit(1);
    }

    if (argc < 2) {
        fprintf(stderr, "Использование: %s имя_файла\n", argv[0]);
        exit(1);
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror("open");
        exit(1);
    }

    lines[line_count].offset = 0;
    lines[line_count].length = 0;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        for (ssize_t i = 0; i < bytes_read; i++) {
            lines[line_count].length++;
            position++;

            if (buffer[i] == '\n') {
                line_count++;

                if (line_count >= array_size) {
                    array_size *= 2;
                    LineInfo *temp = realloc(lines, array_size * sizeof(LineInfo));
                    if (!temp) {
                        perror("realloc");
                        free(lines);
                        close(fd);
                        exit(1);
                    }
                    lines = temp;
                }


                lines[line_count].offset = position;
                lines[line_count].length = 0;
            }
        }
    }


    if (lines[line_count].length > 0) {
        line_count++;
    }

    printf("Номер строки\tСмещение\tДлина\n");
    for (size_t i = 0; i < line_count; i++) {
        printf("%zu\t\t%lld\t\t%zu\n", i + 1, (long long)lines[i].offset, lines[i].length);
    }

    int line_number;
    while (1) {
        printf("Введите номер строки (0 для выхода): ");
        if (scanf("%d", &line_number) != 1) {
            fprintf(stderr, "Некорректный ввод\n");
            break;
        }
        if (line_number == 0) {
            break;
        }
        if (line_number < 1 || line_number > (int)line_count) {
            printf("Неверный номер строки. Пожалуйста, введите число от 1 до %zu\n", line_count);
            continue;
        }

        off_t offset = lines[line_number - 1].offset;
        size_t length = lines[line_number - 1].length;

        char *line_buffer = malloc(length + 1);
        if (!line_buffer) {
            perror("malloc");
            break;
        }

        if (lseek(fd, offset, SEEK_SET) == (off_t)-1) {
            perror("lseek");
            free(line_buffer);
            break;
        }

        ssize_t n = read(fd, line_buffer, length);
        if (n != (ssize_t)length) {
            perror("read");
            free(line_buffer);
            break;
        }

        line_buffer[length] = '\0';

        printf("Строка %d: %s", line_number, line_buffer);

        free(line_buffer);
    }

    free(lines);
    close(fd);
    return 0;
}
