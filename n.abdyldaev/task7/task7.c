#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>

char* buffer;

void catch_alarm(int sig) {
    printf("\nВремя вышло.\n");
    printf("%s", buffer);
    exit(0);
}

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

    struct stat file_stat;
    if (fstat(file, &file_stat) == -1) {
        perror("Ошибка получения информации о файле");
        close(file);
        return 1;
    }
    size_t file_size = file_stat.st_size;

    buffer = (char*) mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, file, 0);
    if (buffer == MAP_FAILED) {
        perror("Ошибка mmap");
        close(file);
        return 1;
    }


    int* table_of_otstup = (int*)malloc(100 * sizeof(int));
    int* table_of_length = (int*)malloc(100 * sizeof(int));
    table_of_otstup[0] = 0;
    int otstup = 0;
    int count_otstup = 1;
    int count_length = 0;
    for (int i = 0; i !=file_size; i++) {
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

    struct sigaction sa;
    sa.sa_handler = catch_alarm;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);
    while (1) {
        alarm(5);
        printf("Введите номер строки или 0 для завершения: ");

        if (scanf("%d", &num) != 1) {
            printf("Ошибка ввода\n");
            while (getchar() != '\n');
            continue;
        }

        if (num == 0) {
            break;
        }

        alarm(0);

        char* string = (char*)malloc(table_of_length[num - 1] + 1);
        strncpy(string, buffer+table_of_otstup[num - 1], table_of_length[num - 1]);
        string[table_of_length[num - 1]] = '\0';
        printf("%s\n", string);
        free(string);
    }

    free(buffer);
    free(table_of_otstup);
    free(table_of_length);
    close(file);

    return 0;
}
