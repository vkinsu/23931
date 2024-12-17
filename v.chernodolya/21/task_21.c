#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <termios.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;
volatile sig_atomic_t sigquit_received = 0;

void handle_sigint(int sig) {
    sigint_count++;
    printf("\a"); // Издает звуковой сигнал
    fflush(stdout);
}

void handle_sigquit(int sig) {
    sigquit_received = 1;
}

int main() {
    struct sigaction sa_int, sa_quit;

    // Устанавливаем обработчик для SIGINT
    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    // Устанавливаем обработчик для SIGQUIT
    sa_quit.sa_handler = handle_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_quit, NULL);

    // Входим в бесконечный цикл
    while (!sigquit_received) {
        pause(); // Ожидаем сигналы
    }

    // Выводим сообщение о количестве полученных SIGINT
    printf("SIGINT received %d times\n", sigint_count);

    return 0;
}