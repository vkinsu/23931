#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {

    if (argc < 2) {
       printf("Not valid argc\n");
       exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("execмvp");
        exit(EXIT_FAILURE);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf("Child process ended with: %d\n", WEXITSTATUS(status));
        } else if(WIFSIGNALED(status)){
            printf("Child proc terminated by: %d\n", WTERMSIG(status));
        } else {
            printf("Fail.\n");
        }

        printf("Parent process\n");
    }
    return 0;
}
