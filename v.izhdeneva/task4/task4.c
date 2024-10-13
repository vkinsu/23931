#include <stdio.h>
#include <stdlib.h>

#define array_size 100
#define string_size 50

// исправить - ТУТ НУЖЕН СПИСОК А НЕ ДВУМЕРНЫЙ МАССИВ


 
int main() {
    int array_len = 0;

    char** string_array = (char**)malloc(array_size * sizeof(char*));

    while (1) {
        char* string = (char*)malloc(string_size * sizeof(char));

        fgets(string, string_size, stdin);

        string_array[array_len] = string;

        if (string[0] == '.') break;

        array_len++;
    }

    for (int i = 0; i < array_len; i++) {
        printf("%d: %s\n", i, string_array[i]);
    }

    return 0;
}
