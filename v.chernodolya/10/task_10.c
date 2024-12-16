#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <spawn.h>
#include <unistd.h>
#include <errno.h>


int main(int argc, char* argv[]) {
    if (argc == 1) {
        printf("No filename.\n");
        return 1;
    }
    printf("Parent pid is: %i\n", getpid());
    pid_t child;
    int status;

    child = fork();
    if (child == (-1)) {
        printf("Error in child process creating.");
        return 1;
    }
    else if (!child) {
        printf("Child process has been created with pid: %i\n", getpid());
        execvp(argv[1], &argv[1]);
        printf("Finish child process.\n");
        exit(10);
    }
    else if(child) {
        printf("\nWait for the child\n");
        waitpid(child, &status, 0);
        printf("\nChild process is ended with exit code %d.\n", WEXITSTATUS(status));
    }
    return 0;
}
