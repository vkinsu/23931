#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>


int main(){

    pid_t pid = fork();
    if (pid < 0) {
        perror("Failed fork");
        return 1;
    } else if (pid == 0) {
        execlp("cat", "cat", "longfile.txt", NULL);
        perror("execlp failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        printf("Child process finished with status %d\n", WEXITSTATUS(status));
        printf("Parent process is running after child.\n");
    }

    return 0;
}
