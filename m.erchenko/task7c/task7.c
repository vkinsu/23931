#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <string.h>

typedef struct {
    off_t offset;
    size_t length;
} LineInfo;

int main(int argc, char *argv[]) {
    int fd;
    struct stat st;
    char *mapped_file;
    size_t file_size;
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

    if (fstat(fd, &st) < 0) {
        perror("fstat");
        close(fd);
        exit(1);
    }

    file_size = st.st_size;

    mapped_file = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped_file == MAP_FAILED) {
        perror("mmap");
        close(fd);
        exit(1);
    }

    close(fd);

    lines[line_count].offset = 0;
    lines[line_count].length = 0;

    for (size_t i = 0; i < file_size; i++) {
        lines[line_count].length++;
        if (mapped_file[i] == '\n') {
            line_count++;
            if (line_count >= array_size) {
                array_size *= 2;
                LineInfo *temp = realloc(lines, array_size * sizeof(LineInfo));
                if (!temp) {
                    perror("realloc");
                    munmap(mapped_file, file_size);
                    free(lines);
                    exit(1);
                }
                lines = temp;
            }
            lines[line_count].offset = i + 1;
            lines[line_count].length = 0;
        }
    }

    if (lines[line_count].length > 0) {
        line_count++;
    }

    printf("Номер строки\tСмещение\tДлина\n");
    for (size_t i = 0; i < line_count; i++) {
        printf("%zu\t\t%lld\t\t%zu\n", i + 1, (long long)lines[i].offset, lines[i].length);
    }

    int line_number = 0;
    while (1) {
        printf("Введите номер строки (0 для выхода): ");
        fflush(stdout);

        fd_set readfds;
        struct timeval timeout;
        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int retval = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (retval == -1) {
            perror("select");
            break;
        } else if (retval == 0) {
            printf("\nВремя ожидания истекло. Выводим содержимое файла:\n\n");
            write(STDOUT_FILENO, mapped_file, file_size);
            break;
        } else {
            char input_buffer[100];
            ssize_t input_length = read(STDIN_FILENO, input_buffer, sizeof(input_buffer) - 1);
            if (input_length > 0) {
                input_buffer[input_length] = '\0';
                char *newline = strchr(input_buffer, '\n');
                if (newline) {
                    *newline = '\0';
                }

                line_number = atoi(input_buffer);
                if (line_number == 0) {
                    break;
                }
                if (line_number < 1 || line_number > (int)line_count) {
                    printf("Неверный номер строки. Пожалуйста, введите число от 1 до %zu\n", line_count);
                    continue;
                }

                off_t offset = lines[line_number - 1].offset;
                size_t length = lines[line_number - 1].length;

                printf("Строка %d: ", line_number);
                fwrite(mapped_file + offset, 1, length, stdout);
                printf("\n");
            } else {
                printf("Ошибка ввода\n");
                break;
            }
        }
    }

    munmap(mapped_file, file_size);
    free(lines);
    return 0;
}
