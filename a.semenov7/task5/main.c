#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>

int main()
{
    int fd = open("test.txt", O_RDONLY); //Открываем файл на чтение
    if (fd == -1)
    {
        printf("Your file was open incorrectly. Pls, stop breaking my programm...\n");
        exit(0);
    }
    int lines[200] = {0}; //Тут храним номера строк
    int lenght[200] = {0}; //А тут длину строк по номерам
    lines[1] = 0L; //Определяем начало первой строки
    int counter = 1; //Тут счётчик, отслеживающий номер строки
    int len = 0; //длина каждой новой строки идёт сюда
    char buf, text[1024]; //buf хранит символы из файла, в text будут записываться итоговые строки
    int test;
    while(read(fd, &buf, 1) != 0) //читаем файл посимвольно, ищем символ переноса строки
    {
        //printf("%c", buf);
        if(buf == '\n') //нашли символ переноса
        {
            len++;
            test = lseek(fd, 0L, 1); //смотрим, сколько бит до начала файла
            lenght[counter] = len; //сюда кидаем длину строки
            counter++; //это нужно, т.к. lines ведет отсчет от следующей строки относительно lenght
            lines[counter] = test; //а тут расстояние от начала
            //printf("%d %d %d %d\n", counter, test, lines[counter], lenght[counter-1]); 
            len = 0; //зануляем счетчик длины
            continue; //запускаем новую итерацию
        }
        len++; //тут понятно
    }

    //printf("\n");

    int temp; //сюда будет записываться номер строки, которую хочет вывести пользователь
    while(1)
    {
        printf("Give me some line numbers: "); 
        int number = scanf("%d", &temp);

        if(number == 0 && (int)number > 199) //если номер ноль, то прога завершается
        {
            printf("Error\n");
            close(fd);
            return -1;
        }
        if(lenght[number] == 0)
        {
            printf("There is no such string\n");
            close(fd);
            return -1;
        }
        
        lseek(fd, lines[number], 0); //ищем расстояние начала строки от начала файла
        //printf("%d\n", test2);
        if(read(fd, text, lenght[number]))
        {
            write(1, text, lenght[number]);
        }
        else
        {
            printf("Error\n");
            close(fd);
            return -1;
        }
    }
    close(fd);
    return 0;


}
