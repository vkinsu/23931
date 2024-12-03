#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


int sigint_count = 0;

void handle_sigint(int sig) {
    sigint_count++;
    write(STDOUT_FILENO, "\a", 1); // Издаем звук
    fflush(stdout);
    signal(SIGINT, handle_sigint);
}

void handle_sigquit(int sig) {
    printf("\nSignal SIGINT was received %d times.\n", sigint_count);
    exit(0);
}

int main() {
    printf("To make sound press Ctrl+C.\n");
    printf("To exit program press Ctrl+\\.\n");

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    while (1) {
        pause(); // Ожидание сигнала
    }
    return 0;
}
