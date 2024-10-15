#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

int lock_file(int fd) {
    struct flock lock;

    lock.l_type = F_WRLCK;
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;

    if (fcntl(fd, F_SETLK, &lock) == -1) {
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Specify a filename\n");
        exit(1);
    }

    const char *filename = argv[1];
    int fd = open(filename, O_RDWR);
    if (fd == -1) {
        printf("Failed to open file");
        exit(-1);
    }

    if (lock_file(fd) != 0) {
        printf("Failed to lock file\n");
        close(fd);
        exit(-1);
    }

    printf("File is locked\n");

    char command[256];
    snprintf(command, sizeof(command), "nano %s", filename);

    int return_value = system(command);

    close(fd);

    if (return_value == -1) {
        printf("Failed to run command");
        exit(-1);
    }

    printf("File unlocked\n");

    return 0;
}
