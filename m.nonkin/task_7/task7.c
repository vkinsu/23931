#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>

void printEntireFile(const char *file_content, size_t file_size) {
    write(STDOUT_FILENO, file_content, file_size);
}

int main(int argc, char *argv[]) {
    int fd;
    // для размера файла в байтах
    struct stat sb;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }

    if (fstat(fd, &sb) == -1) {
        perror("Failed to get file size");
        close(fd);
        exit(EXIT_FAILURE);
    }

    size_t file_size = sb.st_size;

    char *file_content = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_content == MAP_FAILED) {
        perror("Failed to map file");
        close(fd);
        exit(EXIT_FAILURE);
    }

    int len_of_str[500];
    len_of_str[0] = 0;
    int line_count = 1;
    size_t byte_count = 0;

    for (size_t i = 0; i < file_size; ++i) {
        byte_count++;
        if (file_content[i] == '\n') {
            len_of_str[line_count++] = byte_count;
        }
    }

    if (byte_count > len_of_str[line_count - 1]) {
        len_of_str[line_count++] = byte_count;
    }

    // печать строки по номеру
    int line_num;
    while (1) {
        printf("Enter line number (0 to quit): ");
        // не выводило сообщение моментально - это помогло
        fflush(stdout);

        // настройка тайм-аута
        fd_set set;
        struct timeval timeout;
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);

        timeout.tv_sec = 5;
        timeout.tv_usec = 0;

        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        // ошибка
        if (rv == -1) {
            perror("select");
        // тайм-аут йок
        } else if (rv == 0) {
            printf("\nTime out! Printing entire file:\n");
            printEntireFile(file_content, file_size);
            printf("\n");
            break;
        } else {
            if (scanf("%d", &line_num) != 1) {
                fprintf(stderr, "Invalid input\n");
                continue;
            }

            if (line_num == 0) {
                break;
            }

            if (line_num >= line_count) {
                fprintf(stderr, "Line number out of range\n");
                continue;
            }

            int start_position = (line_num > 1) ? len_of_str[line_num - 1] : 0;
            int end_position = len_of_str[line_num];
            int line_length = end_position - start_position;

            write(STDOUT_FILENO, file_content + start_position, line_length);
        }
    }

    munmap(file_content, file_size);
    close(fd);
    return 0;
}

