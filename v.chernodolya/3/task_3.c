#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define FILE_NAME "input.txt"

void open_file() {
    FILE *f = fopen(FILE_NAME, "r");
    if (f == NULL) {
        perror("Error opening file");
    } else {
        fclose(f);
        printf("File opened and closed successfully\n");
    }
}

int main() {
    printf("uid: %d\neuid: %d\n", getuid(), geteuid());

    open_file();

    if (setuid(getuid()) == -1) {
        perror("Error setting user ID");
        return 1;
    }

    printf("uid: %d\neuid: %d\n", getuid(), geteuid());

    open_file();

    return 0;
}
