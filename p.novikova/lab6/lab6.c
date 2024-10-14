#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>

#define MAX_LINES 500
#define BUFFER_SIZE 257

int fd;
long lines_pos[MAX_LINES];
int line_ln[MAX_LINES];
int i;

void print_file_content() {
    lseek(fd, 0, SEEK_SET);
    char buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, BUFFER_SIZE - 1)) > 0) {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }
    close(fd);
    exit(0);
}

void alarm_handler(int sig) {
    printf("\nTime's up!\n");
    print_file_content();
}

int main(int argc, char *argv[]) {
    int j = 0, line_number;
    char c, buf[BUFFER_SIZE];

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Error opening file");
        exit(1);
    }

    lines_pos[0] = 0L;  // Initialize for the first line
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
        fprintf(stderr, "File contains more than %d lines\n", MAX_LINES - 1);
        close(fd);
        exit(1);
    }

    // Output the table of offsets and line lengths for verification
    //for (int k = 0; k < i; k++) {
    //    printf("Line %d: offset=%ld, length=%d\n", k + 1, lines_pos[k], line_ln[k]);
    //}

    // Set signal handler for SIGALRM
    signal(SIGALRM, alarm_handler);

    while (1) {
        printf("You have 5 seconds to input the number.\n");
        printf("Enter the line number (0 to exit): ");
        alarm(5);  // Set timer for 5 seconds
        int result = scanf("%d", &line_number);
        alarm(0);  // Cancel timer after input

        if (result == 0) {
            // Clear input buffer
            while (getchar() != '\n');
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        if (line_number == 0) {
            break;
        }

        if (line_number < 1 || line_number > i) {
            printf("Invalid line number. Available lines: 1-%d\n", i);
            continue;
        }

        if (lseek(fd, lines_pos[line_number - 1], SEEK_SET) == -1) {
            perror("Error positioning in file");
            continue;
        }

        if (read(fd, buf, line_ln[line_number - 1]) == -1) {
            perror("Error reading file");
            continue;
        }

        buf[line_ln[line_number - 1]] = '\0';
        printf("Line %d: %s\n", line_number, buf);
    }

    close(fd);
    return 0;
}