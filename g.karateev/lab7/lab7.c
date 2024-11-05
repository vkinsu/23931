#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
#include <sys/stat.h>

#define MAX_LINE_LENGTH 256

int fd;          
char *file_data; 
struct stat st; 

// Функция для построения таблицы отступов и длин строк
void build_offsets_table(off_t **offsets, size_t **lengths, size_t *num_lines) {
    off_t current_offset = 0;
    *num_lines = 0;

    *offsets = (off_t *)malloc(sizeof(off_t));
    *lengths = (size_t *)malloc(sizeof(size_t));

    size_t line_length = 0;

    // Обработка файла посимвольно через отображенную память
    for (off_t i = 0; i < st.st_size; i++) {
        line_length++;

        // Обнаружен символ новой строки '\n'
        if (file_data[i] == '\n') {
            (*offsets)[*num_lines] = current_offset;
            (*lengths)[*num_lines] = line_length;

            (*num_lines)++;
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
        (*num_lines)++;
    }
}

// Функция для печати строки по номеру
void print_line(off_t offset, size_t length) {
    char buffer[MAX_LINE_LENGTH];

    // Копируем строку из отображенной памяти
    memcpy(buffer, file_data + offset, length);
    buffer[length] = '\0';  // Добавляем завершающий нуль
    printf("%s", buffer);   // Печатаем строку
}

// Обработчик сигнала для завершения программы и печати файла
void handle_alarm(int sig) {
    printf("\nTime's up! Printing entire file...\n");

    // Печать всего файла из отображенной памяти
    printf("\nFile contents:\n");
    fwrite(file_data, 1, st.st_size, stdout); 

    // Закрытие файла и освобождение памяти
    munmap(file_data, st.st_size);
    close(fd);
    exit(0);  // Завершение программы
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(1);
    }

    // Открытие файла
    fd = open(argv[1], O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        exit(1);
    }

    // Получаем информацию о размере файла
    if (fstat(fd, &st) == -1) {
        perror("Error getting file size");
        exit(1);
    }

    // Отображаем файл в память
    file_data = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (file_data == MAP_FAILED) {
        perror("Error mapping file");
        exit(1);
    }

    // Строим таблицу отступов и длин строк
    off_t *offsets = NULL;
    size_t *lengths = NULL;
    size_t num_lines = 0;
    build_offsets_table(&offsets, &lengths, &num_lines);

    // Печать таблицы для отладки
    printf("Offsets table:\n");
    for (size_t i = 0; i < num_lines; i++) {
        printf("Line %zu: Offset = %ld, Length = %zu\n", i + 1, offsets[i], lengths[i]);
    }

    // Установка обработчика сигнала
    signal(SIGALRM, handle_alarm);

    while (1) {
        printf("Enter line number (0 to quit): ");
        
        // Установка таймера на 5 секунд
        alarm(5);
        
        int line_number;
        if (scanf("%d", &line_number) != 1) {
            // Если ввод некорректный, сбрасываем alarm
            alarm(0);
            fprintf(stderr, "Invalid input.\n");
            while (getchar() != '\n'); // Очистка входного потока
            continue;
        }

        alarm(0);  // Сброс таймера

        if (line_number == 0) {
            break;  // Завершаем работу программы
        } else if (line_number > 0 && line_number <= num_lines) {
            print_line(offsets[line_number - 1], lengths[line_number - 1]);  // Печатаем строку
        } else {
            printf("Invalid line number.\n");
        }
    }

    // Освобождаем память и закрываем файл
    free(offsets);
    free(lengths);
    munmap(file_data, st.st_size);  // Освобождение отображенной памяти
    close(fd);

    return 0;
}
