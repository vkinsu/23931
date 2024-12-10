#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static volatile sig_atomic_t beep_count = 0;

void handle_sigint(int signo) {
    (void)signo; // чтобы избежать предупреждений о неиспользуемой переменной
    beep_count++;
    printf("\a"); // выводим звуковой сигнал
    fflush(stdout);
}

void handle_sigquit(int signo) {
    (void)signo;
    printf("\nСигнал прозвучал %d раз(а).\n", beep_count);
    exit(0);
}

int main(void) {
    struct sigaction sa_int, sa_quit;

    sa_int.sa_handler = handle_sigint;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    sa_quit.sa_handler = handle_sigquit;
    sigemptyset(&sa_quit.sa_mask);
    sa_quit.sa_flags = 0;
    if (sigaction(SIGQUIT, &sa_quit, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    while (1) {
        pause();
    }

    return 0;
}
