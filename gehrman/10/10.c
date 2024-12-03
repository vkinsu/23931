#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    // for (int i = 0; i < argc; i++)
    // {
    //     printf("%d %s\n", i, argv[i]);
    // }
    pid_t pid = fork();

    if (pid == 0)
    {
        if (execvp(argv[1], &argv[1]) == -1)
        {
            perror("execvp exited with error code -1\n");
            exit(1);
        }
        // child processs
    }
    else if (pid > 0)
    {
        // parent process
        int exitCode;
        pid_t id = wait(&exitCode);

        printf("Command \"%s\" (pid: %d) exited with code %d\n", argv[1], id, exitCode);
    }
    else
    {
        perror("child process was not created\n");
    }

    return 0;
}

// parametr не передается правильно
// a: правильно передается