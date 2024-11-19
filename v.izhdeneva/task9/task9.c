#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (!pid) {
        int rv = execlp("cat", "cat", "input.txt", NULL);

        if (rv == -1) {
            perror("execlp");
            exit(1);
        }
    } else if (pid > 0) {
        printf("\nSubprocess has started.\n\n");
        pid = wait(NULL);
        printf("\nSubprocess has ended.\n");

        if (!pid) {
            perror("wait");
            exit(1);
        }
    } else {
        perror("fork");
        exit(1);
    }



    exit(0);
}