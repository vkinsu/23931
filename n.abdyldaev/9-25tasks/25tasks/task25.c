#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>


int main() {
    int pipe_fd[2];
    pid_t pid;

    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipe_fd[1]);

        char buffer[1024];
        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';
            for (int i = 0; buffer[i] != '\0'; i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }
            printf("Child: %s\n", buffer);
        } else {
            perror("read");
        }
        close(pipe_fd[0]);
    } else {
        close(pipe_fd[0]);
        const char *text = "Hello, World!";
        printf("Parent: %s\n", text);
        if (write(pipe_fd[1], text, strlen(text)) == -1) {
            perror("write");
        }
        close(pipe_fd[1]);
    }

    return 0;
}

