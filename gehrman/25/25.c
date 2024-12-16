#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int main()
{
    char message[20];
    fgets(message, 20, stdin);

    int fd[2]; // File descriptors for the pipe
    pipe(fd);  // Create the pipe

    pid_t pid = fork();

    if (pid == 0)
    {
        // Child process
        close(fd[1]); // Close unused write end
        char buffer[128];
        read(fd[0], buffer, sizeof(buffer)); // Read from pipe
        printf("Child received: ");
        int i = 0;
        while (buffer[i])
        {
            printf("%c", toupper(buffer[i]));
            i++;
        }
        close(fd[0]);
    }
    else if (pid > 0)
    {
        // Parent process
        printf("Parent sending: %s\n", message);
        close(fd[0]);                             // Close unused read end
        write(fd[1], message, 20 * sizeof(char)); // Write to pipe
        close(fd[1]);
    }
    else
    {
        perror("child process was not created\n");
    }

    return 0;
}

// почему надо закрывать каналы
// а: тк надо