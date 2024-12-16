#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    // argv[1] - команда; все, что далее это аргументы к команде
    if (argc < 2) {
        perror("No arguments\n");
        exit(1);
    }

    pid_t pid = fork();

    if (!pid) {
        int rv = execvp(argv[1], &argv[1]);

        if (rv == -1) {
            perror("execvp");
            exit(-1);
        }
    } else if (pid == -1) {
        perror("fork");
        exit(-1);
    } else {
        pid = wait(NULL);

        if (pid == -1) {
            perror("wait");
            return -1;
        }
    }

    return 0;
}
