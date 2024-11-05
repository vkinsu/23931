#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

typedef struct Line {
    int len;
    int init;
}Line;

typedef struct Vector {
    Line* string;
    int cap;
    int size;
}Vector;

Vector initVector() {
    Vector A;
    A.string = malloc(sizeof(Line));
    A.cap = 1;
    A.size = 0;
    return A;
}

void delete(Vector* A) {
    if (A) free(A->string);
}

void add(Vector* A, Line* line) {
    if (A == NULL) exit(1);
    if (A->cap == A->size) {
        A->cap *= 2;
        A->string = realloc(A->string, A->cap * sizeof(Line));
        if (A->string == NULL) exit(1);
    }
    A->string[A->size++] = *line;
}

Vector createTable(int file) {
    Vector table = initVector();
    char c;
    int init = 0, len = 0;
    while (1) {
        int r = read(file, &c, sizeof(char));
        if (c == '\n' || r == 0) {
            Line new_line = { len, init };
            add(&table, &new_line);
            init += len + 1;
            len = 0;
            if (r == 0) break;
        }
        else len++;
    }
    return table;
}

int main() {
    int file = open("file.txt", O_RDONLY);
    if (file == -1) exit(1);

    Vector table = createTable(file);
    long num = 1;

    while (1) {
        printf("Enter a line: ");
        char input[32];
        fgets(input, 32, stdin);
        num = strtol(input, 0, 10);
        if (num == 0) break;
        if (num < 0 || num > table.size) exit(1);
        lseek(file, table.string[num - 1].init, 0);
        char* str = malloc(table.string[num - 1].len * sizeof(char));
        read(file, str, table.string[num - 1].len);
        str[table.string[num - 1].len] = '\0';
        printf("%s\n", str);
    }
    delete(&table);
    close(file);
    return 0;
}
