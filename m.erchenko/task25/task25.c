#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    int fd[2];
    pid_t pid;
    char *text = "Hello World! heLLowOrld\n";
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        close(fd[0]);
        write(fd[1], text, strlen(text));
        close(fd[1]);
        wait(NULL);
    } else {
        close(fd[1]);
        char buffer[1024];
        ssize_t n = read(fd[0], buffer, sizeof(buffer)-1);
        if (n < 0) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        buffer[n] = '\0';
        close(fd[0]);
        for (int i = 0; buffer[i] != '\0'; i++) {
            buffer[i] = (char)toupper((unsigned char)buffer[i]);
        }
        printf("%s", buffer);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }
    return 0;
}
