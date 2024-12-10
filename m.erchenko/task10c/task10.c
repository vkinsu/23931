#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;

    if (argc < 2) {
        fprintf(stderr, "Использование: %s команда [аргументы...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
        execvp(argv[1], &argv[1]);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        printf("Родительский процесс: PID = %d, дочерний PID = %d\n", getpid(), pid);
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        if (WIFEXITED(status)) {
            int exit_status = WEXITSTATUS(status);
            printf("Процесс завершился с кодом %d\n", exit_status);
        } else if (WIFSIGNALED(status)) {
            int signal = WTERMSIG(status);
            printf("Процесс был завершен сигналом %d\n", signal);
        } else {
            printf("Процесс завершился аномально\n");
        }
    }

    return 0;
}
