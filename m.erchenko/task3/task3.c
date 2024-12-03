#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h> 
#include <stdlib.h>

int main(int argc, char *argv[]) {
    const char *name = argv[1];
    struct stat file_stat;
    stat(name, &file_stat);

    FILE *file = fopen(name, "r");
    if (file == NULL) {
        perror("Ошибка");
        exit(EXIT_FAILURE);
    } else {
        fclose(file);
        printf("123 closed\n");
    }

    uid_t real = getuid();
    uid_t effective = geteuid();

    printf("%d\n", real);
    printf("%d\n", effective);

    setuid(real);

    printf("%d\n", real);
    printf("%d\n", effective);

    file = fopen(name, "r");
    if (file == NULL) {
        perror("Ошибка");
        exit(EXIT_FAILURE);
    } else {
        fclose(file);
        printf("123 closed\n");
    }
}