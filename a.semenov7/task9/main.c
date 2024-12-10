#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    pid_t pid;
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
        execlp("cat", "cat", "file.txt", NULL);
    }
    else if (pid > 0)
    {
        printf("I am papa.\n");
        int returnStatus;    
        waitpid(pid, &returnStatus, 0); 
        if(returnStatus == 0)
        {
            printf("MY CHILD!!!\n");
        }
    }
    return(0);
}
