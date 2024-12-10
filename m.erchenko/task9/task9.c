#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
    pid_t pid;
    int status;

    if (argc != 2) {
        fprintf(stderr, "Использование: %s <имя_файла>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t parent_pid = getpid();
    printf("Родительский процесс PID: %d\n", parent_pid);

    pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        pid_t child_pid = getpid();
        pid_t child_ppid = getppid();
        printf("Дочерний процесс PID: %d, PPID: %d\n", child_pid, child_ppid);
        execlp("cat", "cat", argv[1], (char *)NULL);
        perror("execlp");
        exit(EXIT_FAILURE);
    } else {
        printf("Родительский процесс продолжает работу. PID: %d, Дочерний PID: %d\n", parent_pid, pid);

        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }

        printf("Дочерний процесс завершён. Это последнее сообщение от родительского процесса.\n");
    }

    return 0;
}
