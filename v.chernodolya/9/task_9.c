#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        // Ошибка при создании дочернего процесса
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // Дочерний процесс
        printf("Дочерний процесс выполняется\n");
        execlp("cat", "cat", "test.txt", NULL);
        // Если execlp не удалось, выводим сообщение об ошибке
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        // Родительский процесс
        printf("Родительский процесс выполняется\n");

        // Ожидание завершения дочернего процесса
        int status;
        pid_t result = waitpid(pid, &status, 0);
        if (result == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        // Проверка статуса завершения дочернего процесса
        if (WIFEXITED(status)) {
            printf("\nДочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            printf("\nДочерний процесс завершился из-за сигнала %d\n", WTERMSIG(status));
        } else {
            printf("\nДочерний процесс завершился неожиданно\n");
        }
    }

    return 0;
}
