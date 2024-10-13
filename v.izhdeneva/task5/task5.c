#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

#define MAX_N 100
#define MAX_M 256

int main() {
    int fd = open("input.txt", O_RDONLY);

    if (fd == -1) {
        printf("Can't open the file\n");
        return -1;
    }

    off_t offsets[MAX_N];
    int line_count = 0;

    char buffer[MAX_M];
    off_t current_offset = 0;
    ssize_t bytesRead;

    while ((bytesRead = read(fd, buffer, MAX_M)) > 0) {
        for (ssize_t i = 0; i < bytesRead; i++) {
            if (line_count == 0|| buffer[i] == '\n') {
                offsets[line_count++] = current_offset + i + 1;
            }
        }
        current_offset += bytesRead;
    }

    close(fd);
    offsets[0] = 0;
    line_count--;

    // for (int i = 0; i < line_count; i++) printf("String %d starts from byte %ld\n", i + 1, offsets[i]);
    
    fd = open("input.txt", O_RDONLY);

    if (fd == -1) {
        printf("Can't open the file\n");
        return -1;
    }

    int string_num;

    while (1) {
        printf("Enter the number of string: ");
        scanf("%d", &string_num);

        if (string_num == 0) {
            return 0;
        }

        if (string_num < 1 || string_num > line_count) {
            printf("The wrong number of a string\n");
            return -1;
        }

        if (lseek(fd, offsets[string_num - 1], SEEK_SET) == (off_t)-1) {
            printf("Can't read string\n");
            close(fd);
            return -1;
        }

        ssize_t bytesRead = read(fd, buffer, MAX_M);

        for (ssize_t i = 0; i < bytesRead; i++) {
            if (buffer[i] == '\n') {
                buffer[i] = '\0'; 
                break;
            }
        }

        printf("String %d: %s\n", string_num, buffer); 
    }

    close(fd);

    return 0;
}
