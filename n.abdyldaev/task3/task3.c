#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
void open_file(const char *filename) {
    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error opening file");
    } else {
        printf("File opened successfully.\n");
        fclose(file);
    }
}

int main() {
    const char *filename = "myfile";

    printf("Real uid: %d\n", getuid());
    printf("Effective uid: %d\n", geteuid());

    FILE *file = fopen(filename, "r");

    if (file == NULL) {
        perror("Error");
    } else {
        printf("File opened successfully\n");
        fclose(file);
    }

    if (setuid(getuid()) == -1) {
        perror("Error");
        exit(EXIT_FAILURE);
    }
    printf("Real uid: %d\n", getuid());
    printf("Effective uid: %d\n", geteuid());

    return 0;
}

