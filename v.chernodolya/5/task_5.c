#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define MAX_SIZE 1000

typedef struct line_info {
    int indentation;
    int length;
} line_info;

line_info table[MAX_SIZE];
int count = 0;

void array_add(line_info* data) {
    if (count < MAX_SIZE) {
        table[count++] = *data;
    }
}

void build_table(int descriptor) {
    char symbol;
    int indentation = 0;
    int length = 0;
    while (1) {
        long f = read(descriptor, &symbol, 1);
        if (symbol == '\n' || f == 0) {
            line_info data = { indentation, length };
            array_add(&data);
            indentation += length + 1;
            length = 0;
            if (f == 0)
                break;
        }
        else
            length++;
    }
}

int main() {
    int descriptor = open("input.txt", O_RDONLY);
    if (descriptor == -1) {
        perror("");
        exit(1);
    }
    build_table(descriptor);
    while (1) {
        printf("Enter the number of a line: ");
        char digits[32];
        fgets(digits, 32, stdin);
        long number = strtol(digits, NULL, 10);
        if (number == 0)
            break;
        if (0 < number && number <= count) {
            lseek(descriptor, table[number - 1].indentation, SEEK_SET);
            char line[table[number - 1].length];
            read(descriptor, line, table[number - 1].length);
            line[table[number - 1].length] = 0;
            printf("%s\n", line);
        }
    }
    close(descriptor);
    return 0;
}
