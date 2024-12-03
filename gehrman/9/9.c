#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main()
{
    pid_t pid = fork();

    if (pid == 0)
    {
        // child process
        execlp("cat", "cat", "alice29.txt", NULL);
    }
    else if (pid > 0)
    {
        // parent process
        pid_t id = wait(NULL);
        printf("Thank you, child number %d\nAnd now the grownups (%d) are talking\n", id, getpid());
    }
    else
    {
        perror("child process was not created\n");
    }

    return 0;
}