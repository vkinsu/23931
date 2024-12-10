#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    pid_t pid;
    int f;
    if (argc < 2) {
        perror("Not enough arguments.\n");
        return(-1);
    }
    printf("Fork here.\n");
    pid = fork();
    if (pid == -1)
    {
        perror("Error\n");
        return(-1);
    }
    printf("\nFork successful!\n");
    if (pid == 0)
    {
        printf("I am kid.\n");
        execvp(argv[1], &argv[1]);
    }
    else if (pid > 0)
    {
        printf("I am papa.\n");
        if (waitpid(pid, &f, 0) == -1) {
            fprintf(stderr, "waitpid failed");
            exit(1);
        }
        if (WIFEXITED(f)) 
            printf("Child exited with status %d\n", WEXITSTATUS(f));
        else if (WIFSIGNALED(f)) 
            printf("Child terminated by signal %d\n", WTERMSIG(f));
    }
    return(0);
}
