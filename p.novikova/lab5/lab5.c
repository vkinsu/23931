#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX_LINES 500
#define BUFFER_SIZE 257

int main(int argc, char *argv[]) {
    long lines_pos[MAX_LINES];
    int fd;
    int i = 1, j = 0, line_number, line_ln[MAX_LINES];
    char c, buf[BUFFER_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Error opening file");
        exit(1);
    }

    lines_pos[1] = 0L;
    while (read(fd, &c, 1) > 0) {
        if (c == '\n') {
            j++;
            line_ln[i++] = j;
            lines_pos[i] = lseek(fd, 0L, SEEK_CUR);
            j = 0;
        } else {
            j++;
        }
    }

    if (i >= MAX_LINES) {
        fprintf(stderr, "File includes more than %d lines\n", MAX_LINES - 1);
        close(fd);
        exit(1);
    }

    // Вывод таблицы отступов и длин строк для проверки
    //for (int k = 1; k < i; k++) {
    //    printf("Line %d: offset=%ld, length=%d\n", k, lines_pos[k], line_ln[k]);
    //}

    while (1) {
        printf("Enter number of line (0 for exit): ");
        int result = scanf("%d", &line_number);

        if (result == 0) {
            // Очистка буфера ввода
            while (getchar() != '\n');
            printf("Incorrect input. Please, enter a number.\n");
            continue;
        }

        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number >= i) {
            printf("Incorrect number of line. Available lines: 1-%d\n", i - 1);
            continue;
        }

        if (lseek(fd, lines_pos[line_number], SEEK_SET) == -1) {
            perror("Positioning error in file");
            continue;
        }

        if (read(fd, buf, line_ln[line_number]) == -1) {
            perror("Error reading file");
            continue;
        }

        buf[line_ln[line_number]] = '\0';
        printf("Line %d: %s\n", line_number, buf);
    }

    close(fd);
    return 0;
}