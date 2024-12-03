#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Ошибка: нужно указать имя файла.\n");
        return 1;
    }

    int file = open(argv[1], O_RDONLY);
    if (file == -1) {
        perror("Ошибка при открытии файла");
        return 1;
    }

    char* buffer = (char*)malloc(100);
    read(file, buffer, 100);
    int end_of_file = lseek(file, 0, SEEK_END);
    lseek(file, 0, SEEK_SET);

    int* table_of_otstup = (int*)malloc(100 * sizeof(int));
    int* table_of_length = (int*)malloc(100 * sizeof(int));
    table_of_otstup[0] = 0;
    int otstup = 0;
    int count_otstup = 1;
    int count_length = 0;
    for (int i = 0; i != end_of_file; i++) {
        if (buffer[i] == '\n' || buffer[i] == '\0') {
            table_of_otstup[count_otstup] = i + 1;
            table_of_length[count_length] = i - otstup;
            otstup = i + 1;
            count_otstup++;
            count_length++;
        }
    }

    for (int i = 0; i < count_otstup; i++) {
        printf("%d строка: %d отступ и длина: %d\n", i + 1, table_of_otstup[i], table_of_length[i]);
    }

    int num = 1;
    while (1) {
        printf("Введите номер строки или 0 для завершения: ");

        if (scanf("%d", &num) != 1) {
            printf("Ошибка ввода\n");
            while (getchar() != '\n');
            continue;
        }

        if (num == 0) {
            break;
        }

        char* string = (char*)malloc(table_of_length[num - 1] + 1);
        lseek(file, table_of_otstup[num - 1], SEEK_SET);
        read(file, string, table_of_length[num - 1]);
        string[table_of_length[num - 1]] = '\0';
        printf("%s\n", string);
        free(string);

        lseek(file, 0, SEEK_SET);
    }

    free(buffer);
    free(table_of_otstup);
    free(table_of_length);
    close(file);

    return 0;
}

