#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        execlp("cat", "cat", "new.txt", (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("child process finished with exit status %d\n", WEXITSTATUS(status));
        } else {
            printf("child process did not finish normally\n");
        }

        printf("parent process is finished.\n");
    }

    return 0;
}

