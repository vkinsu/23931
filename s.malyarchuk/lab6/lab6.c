#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

char* buf;

void alarm_handler(int sig) {
    printf("\nTimeout.\n");
    printf("%s", buf);
    exit(0);
}

int main(int argc, char *argv[]) {
    int fd = open("input.txt", O_RDONLY);
    if (fd == -1) {
        perror("Ошибка файла");
        return 1;
    }

    buf = (char*)malloc(100);
    read(fd, buf, 100);
    int eof = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);

    int* indents = (int*)malloc(100 * sizeof(int));
    int* lengths = (int*)malloc(100 * sizeof(int));
    indents[0] = 0;
    int pos = 0;
    int ind_count = 1;
    int len_count = 0;
    for (int i = 0; i != eof; i++) {
        if (buf[i] == '\n' || buf[i] == '\0') {
            indents[ind_count] = i + 1;
            lengths[len_count] = i - pos;
            pos = i + 1;
            ind_count++;
            len_count++;
        }
    }


    int line = 1;

    struct sigaction sa;
    sa.sa_handler = alarm_handler;
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);
    while (1) {
        alarm(5);
        printf("Enter some number: ");

        if (scanf("%d", &line) != 1) {
            printf("Ошибка ввода\n");
            while (getchar() != '\n');
            continue;
        }

        if (line == 0) {
            break;
        }

        alarm(0);
        char* line_buf = (char*)malloc(lengths[line - 1] + 1);
        lseek(fd, indents[line - 1], SEEK_SET);
        read(fd, line_buf, lengths[line - 1]);
        line_buf[lengths[line - 1]] = '\0';
        printf("%s\n", line_buf);
        free(line_buf);

        lseek(fd, 0, SEEK_SET);
    }

    free(buf);
    free(indents);
    free(lengths);
    close(fd);

    return 0;
}
