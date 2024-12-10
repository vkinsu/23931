#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

#define BUFFER_SIZE 255

int main() {
    int pipefd[2];
    pid_t pid;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(-1);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(-1);
    }

    if (pid == 0) {
        close(pipefd[1]);

        char buffer[BUFFER_SIZE];
        ssize_t bytes_read;

        while ((bytes_read = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[bytes_read] = '\0';

            for (size_t i = 0; i < strlen(buffer); i++) {
                buffer[i] = toupper((unsigned char)buffer[i]);
            }

            printf("\nText that child process received: %s", buffer);
        }

        if (bytes_read == -1) {
            perror("read");
        }

        close(pipefd[0]);
        exit(0);

    } else {
        close(pipefd[0]);

        const char *message = "\n\nThis is a string for to upper\n\n";
        write(pipefd[1], message, strlen(message));

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
