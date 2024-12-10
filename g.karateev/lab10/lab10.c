#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    if (argc < 2){
        fprintf(stderr, "Usage: %s <command> [args...]\n", argv[0]);
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0){
        perror("Error creating subprocess");
        return 1;
    } else if (pid == 0) {
        execvp(argv[1], &argv[1]);
        perror("Error execvp");
    } else {
        int status;
        waitpid(pid, &status, 0);
        
        if (WIFEXITED(status)) {
            printf("\nChild proccess exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("\nChild process exited abnormally\n");
        }
    }
}