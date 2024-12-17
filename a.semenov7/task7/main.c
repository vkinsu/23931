#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

void timer(int var)
{
    printf("Haha\n");
    exit(0);
}

int main()
{
    int fd = open("test.txt", O_RDONLY); // Открываем файл на чтение
    if (fd == -1)
    {
        printf("Your file was open incorrectly. Pls, stop breaking my programm...\n");
        exit(0);
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("fstat");
        exit(1);
    }

    // Отображаем файл в память
    char *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // Закрываем дескриптор файла, так как он больше не нужен
    close(fd);

    int lines[200]; // Тут храним номера строк
    int length[200]; // А тут длину строк по номерам
    lines[1] = 0L; // Определяем начало первой строки
    int counter = 1; // Тут счётчик, отслеживающий номер строки
    int len = 0; // длина каждой новой строки идёт сюда

    // Обработка строк
    for (size_t i = 0; i < sb.st_size; i++) {
        char buf = mapped[i]; // читаем символ из отображенной памяти
        if (buf == '\n') // нашли символ переноса
        {
            length[counter] = len; // сюда кидаем длину строки
            counter++; // это нужно, т.к. lines ведет отсчет от следующей строки относительно length
            lines[counter] = i + 1; // расстояние от начала
            len = 0; // зануляем счетчик длины
            continue; // запускаем новую итерацию
        }
        len++; // увеличиваем длину строки
    }
    if (len > 0) {
        length[counter] = len; // длина последней строки
        lines[counter + 1] = sb.st_size; // до конца файла
        counter++;
    }

    int number; // сюда будет записываться номер строки, которую хочет вывести пользователь
    signal(SIGALRM, timer);
    alarm(5);
    while (1)
    {
        printf("Give me some line numbers: ");
        scanf("%d", &number);
        if (number == 0) // если номер ноль, то прога завершается
            return -1;

        if (number < 1 || number >= counter) {
            printf("Invalid line number.\n");
            continue;
        }

        // Получаем указатель на начало строки и выводим её
        printf("%.*s\n", length[number], &mapped[lines[number]]);
    }

    // Освобождаем отображение памяти
    munmap(mapped, sb.st_size);
    return 0;
}

