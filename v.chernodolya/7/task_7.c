#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <signal.h>

#define MAX_SIZE 1000

typedef struct line_info {
    int indentation;
    int length;
} line_info;

line_info table[MAX_SIZE];
int count = 0;
char *file_data;

void array_add(line_info* data) {
    if (count < MAX_SIZE) {
        table[count++] = *data;
    }
}

void build_table(char *data) {
    int indentation = 0;
    int length = 0;
    while (*data) {
        if (*data == '\n') {
            line_info data = { indentation, length };
            array_add(&data);
            indentation += length + 1;
            length = 0;
        }
        else
            length++;
        data++;
    }
}

void print_line(int line) {
    printf("%.*s\n", table[line - 1].length, file_data + table[line - 1].indentation);
}

void sig_handler(int _) {
    printf("The content of the file:\n");
    int line = 0;
    for (; line < count; line++)
        print_line(line + 1);
    exit(0);
}

int main() {
    int descriptor = open("input.txt", O_RDONLY);
    if (descriptor == -1) {
        perror("");
        exit(1);
    }
    struct stat file_stat;
    fstat(descriptor, &file_stat);
    file_data = mmap(NULL, file_stat.st_size, PROT_READ, MAP_PRIVATE, descriptor, 0);
    if (file_data == MAP_FAILED) {
        perror("");
        exit(1);
    }
    build_table(file_data);
    signal(SIGALRM, sig_handler);
    while (1) {
        printf("Enter the number of a line: ");
        char digits[32];
        alarm(5);
        fgets(digits, 32, stdin);
        long number = strtol(digits, NULL, 10);
        if (number == 0)
            break;
        if (0 < number && number <= count) {
            print_line(number);
        }
    }
    munmap(file_data, file_stat.st_size);
    close(descriptor);
    return 0;
}
