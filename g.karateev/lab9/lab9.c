#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid;

    pid = fork();

    if (pid < 0) {
        perror("Error creating subprocess");
        return 1;
    } else if (pid == 0) {
        printf("Child process with PID %d started, parent PID is %d\n", getpid(), getppid());

        execlp("cat", "cat", "long.txt", NULL);

        perror("Error executing cat");
        exit(1);
    } else {
        printf("Parent process: child process with PID %d was created.\n", pid);

        int status;
        waitpid(pid, &status, 0);

        printf("\nChild process with PID %d has finished.\n", pid);
    }

    return 0;
}
