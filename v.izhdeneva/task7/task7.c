#include<stdio.h>
#include<stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_N 100
#define MAX_M 256

int main() {
    int fd = open("input.txt", O_RDONLY);

    if (fd == -1) {
        perror("Can't open file\n");
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
        perror("Can't open file\n");
        return -1;
    }

    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("Error with fstat()\n");
        close(fd);
        return 1;
    }

    size_t filesize = st.st_size;

    char *mapped = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("Error with mmap()\n");
        close(fd);
        return 1;
    }

    int string_num;

    while (1) {
        printf("Enter the number of string: ");
        int result = scanf("%d", &string_num);

        if (!result || ((string_num < 1 && string_num != 0) || string_num > line_count)) {
            perror("Wrong input\n");
            close(fd);
            return -1;
        }

        if (string_num == 0) {
            return 0;
        }

        char *str_start = mapped + offsets[string_num - 1];
        char string[MAX_M];
        size_t str_len = 0;

        while (str_len < MAX_M - 1 && str_start[str_len] != '\n' && (str_start[str_len] != '\0')) {
            string[str_len] = str_start[str_len];
            str_len++;
        }

        string[str_len] = '\0';

        printf("String %d: %s\n", string_num, string); 
    }

    munmap(mapped, filesize);
    close(fd);

    return 0;
}
