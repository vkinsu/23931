#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX_LINE_LENGTH 256

// Функция для построения таблицы отступов и длин строк
void build_offsets_table(int fd, off_t **offsets, size_t **lengths, size_t *num_lines) {
    off_t current_offset = 0;
    char ch;
    *num_lines = 0;

    *offsets = (off_t *)malloc(sizeof(off_t));
    *lengths = (size_t *)malloc(sizeof(size_t));

    size_t line_length = 0;

    // Обработка файла посимвольно
    while (read(fd, &ch, 1) == 1) {
        line_length++;

        // Обнаружен символ новой строки '\n'
        if (ch == '\n') {
            (*offsets)[*num_lines] = current_offset;
            (*lengths)[*num_lines] = line_length;

            *num_lines += 1;
            *offsets = (off_t *)realloc(*offsets, (*num_lines + 1) * sizeof(off_t));
            *lengths = (size_t *)realloc(*lengths, (*num_lines + 1) * sizeof(size_t));

            current_offset += line_length;
            line_length = 0;
        }
    }

    // Проверка на последнюю строку без перевода строки
    if (line_length > 0) {
        (*offsets)[*num_lines] = current_offset;
        (*lengths)[*num_lines] = line_length;
        *num_lines += 1;
    }
}

// Функция для печати строки по номеру
void print_line(int fd, off_t offset, size_t length) {
    char buffer[MAX_LINE_LENGTH];

    lseek(fd, offset, SEEK_SET);  // Перемещаем указатель в нужное место
    read(fd, buffer, length);     // Читаем строку нужной длины
    buffer[length] = '\0';        // Добавляем завершающий нуль
    printf("%s", buffer);         // Печатаем строку
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    // Открытие файла
    int fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    // Строим таблицу отступов и длин строк
    off_t *offsets = NULL;
    size_t *lengths = NULL;
    size_t num_lines = 0;
    build_offsets_table(fd, &offsets, &lengths, &num_lines);

    // Печать таблицы для отладки
    printf("Offsets table:\n");
    for (size_t i = 0; i < num_lines; i++) {
        printf("Line %zu: Offset = %ld, Length = %zu\n", i + 1, offsets[i], lengths[i]);
    }

    int line_number;
    while (1) {
        printf("Enter line number (0 to quit): ");
        scanf("%d", &line_number);

        if (line_number == 0) {
            break;  // Завершаем работу программы
        } else if (line_number > 0 && line_number <= num_lines) {
            print_line(fd, offsets[line_number - 1], lengths[line_number - 1]);  // Печатаем строку
        } else {
            printf("Invalid line number.\n");
        }
    }


    // Освобождаем память и закрываем файл
    free(offsets);
    free(lengths);
    close(fd);

    return 0;
}
