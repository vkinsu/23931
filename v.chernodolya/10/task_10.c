#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s command [args...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid < 0) {
        // Ошибка при создании дочернего процесса
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Дочерний процесс
        execvp(argv[1], &argv[1]);
        // Если execvp не удалось, выводим сообщение об ошибке
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
        int status;
        pid_t result = waitpid(pid, &status, 0);
        if (result == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // Проверка статуса завершения дочернего процесса
        if (WIFEXITED(status)) {
            printf("Дочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("Дочерний процесс завершился из-за сигнала %d\n", WTERMSIG(status));
        } else {
            printf("Дочерний процесс завершился неожиданно\n");
        }
    }

    return 0;
}
