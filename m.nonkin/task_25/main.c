#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

int main() {
    int pipefd[2]; 
    pid_t pid;
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid == 0) {
        close(pipefd[1]);
        char buffer[256];
        ssize_t nread;
        while ((nread = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            for (ssize_t i = 0; i < nread; i++) {
                buffer[i] = toupper((unsigned char) buffer[i]);
            }
            write(STDOUT_FILENO, buffer, nread);
        }

        close(pipefd[0]);
        exit(0);
    } else {
        close(pipefd[0]);
        const char *text = "<`exAmple Patte'rn72(,.).\n";
        write(pipefd[1], text, strlen(text));
        close(pipefd[1]);
        exit(0);
    }
}

