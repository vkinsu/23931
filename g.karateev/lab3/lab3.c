#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Проверка наличия аргумента
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char *filename = argv[1];

    // Печать реального и эффективного идентификаторов пользователя
    uid_t real_uid = getuid();
    uid_t effective_uid = geteuid();
    printf("Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);

    // Открытие файла
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
    } else {
        printf("File '%s' opened successfully.\n", filename);
        fclose(file);
    }

    // Сделать реальный и эффективный UID одинаковыми
    if (setuid(real_uid) == -1) {
        perror("Error setting UID");
        exit(EXIT_FAILURE);
    }

    // Повторная печать UID
    effective_uid = geteuid();
    printf("After setuid - Real UID: %d, Effective UID: %d\n", real_uid, effective_uid);

    // Повторное открытие файла
    file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file after setuid");
    } else {
        printf("File '%s' opened successfully after setuid.\n", filename);
        fclose(file);
    }

    return 0;
}
