#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024

int main() {
    int pipe_fd[2];
    pid_t pid;
    char buffer[BUFFER_SIZE];


    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }


    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
       
        close(pipe_fd[1]);
        

        while (read(pipe_fd[0], buffer, sizeof(buffer)) > 0) {
            for (int i = 0; buffer[i] != '\0'; i++) {
                buffer[i] = toupper((unsigned char) buffer[i]);
            }
            printf("Received and converted to uppercase: %s\n", buffer);
        }
        

        close(pipe_fd[0]);
        exit(EXIT_SUCCESS);
    } else {

        close(pipe_fd[0]);
        
        printf("Parent process is writing text to pipe...\n");
        snprintf(buffer, sizeof(buffer), "Hello, World! This is a test of PIPE communication!");
        write(pipe_fd[1], buffer, sizeof(buffer));
        

        close(pipe_fd[1]);
        
        wait(NULL);
        exit(EXIT_SUCCESS);
    }
}
