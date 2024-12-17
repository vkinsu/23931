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
    int fd = open("test.txt", O_RDONLY); // ��������� ���� �� ������
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

    // ���������� ���� � ������
    char *mapped = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }

    // ��������� ���������� �����, ��� ��� �� ������ �� �����
    close(fd);

    int lines[200]; // ��� ������ ������ �����
    int length[200]; // � ��� ����� ����� �� �������
    lines[1] = 0L; // ���������� ������ ������ ������
    int counter = 1; // ��� �ޣ����, ������������� ����� ������
    int len = 0; // ����� ������ ����� ������ �ģ� ����

    // ��������� �����
    for (size_t i = 0; i < sb.st_size; i++) {
        char buf = mapped[i]; // ������ ������ �� ������������ ������
        if (buf == '\n') // ����� ������ ��������
        {
            length[counter] = len; // ���� ������ ����� ������
            counter++; // ��� �����, �.�. lines ����� ������ �� ��������� ������ ������������ length
            lines[counter] = i + 1; // ���������� �� ������
            len = 0; // �������� ������� �����
            continue; // ��������� ����� ��������
        }
        len++; // ����������� ����� ������
    }
    if (len > 0) {
        length[counter] = len; // ����� ��������� ������
        lines[counter + 1] = sb.st_size; // �� ����� �����
        counter++;
    }

    int number; // ���� ����� ������������ ����� ������, ������� ����� ������� ������������
    signal(SIGALRM, timer);
    alarm(5);
    while (1)
    {
        printf("Give me some line numbers: ");
        scanf("%d", &number);
        if (number == 0) // ���� ����� ����, �� ����� �����������
            return -1;

        if (number < 1 || number >= counter) {
            printf("Invalid line number.\n");
            continue;
        }

        // �������� ��������� �� ������ ������ � ������� ţ
        printf("%.*s\n", length[number], &mapped[lines[number]]);
    }

    // ����������� ����������� ������
    munmap(mapped, sb.st_size);
    return 0;
}

