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
    char* args_to_child[] = { "cat", argv[1], NULL };
    pid_t child;

    child = fork();
    if (child == (-1)) {
        printf("Error in child process creating.");
        return 1;
    }
    else if (!child) {
        printf("Child process has been created with pid: %i\n", getpid());
        execv("/bin/cat", args_to_child);//каталог содержащий исполняемые файлы
        printf("Finish child process.\n");
        exit(0);
    }
    else if(child) {
        printf("\nWait for the child\n");
        wait(NULL);
        printf("\nChild process is ended.\n");
    }
    return 0;
}
