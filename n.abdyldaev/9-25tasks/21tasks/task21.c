#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigint_count = 0;

void handle_sigint(int sig) {
    (void)sig;
    printf("\a");
    fflush(stdout);
    sigint_count++;
}

void handle_sigquit(int sig) {
    (void)sig;
    printf("\nTotal beeps: %d.\n", sigint_count);
    exit(0);
}

int main() {
    struct sigaction sa_sigint, sa_sigquit;
    sa_sigint.sa_handler = handle_sigint;
    sigemptyset(&sa_sigint.sa_mask);
    sa_sigint.sa_flags = 0;
    sigaction(SIGINT, &sa_sigint, NULL);

    sa_sigquit.sa_handler = handle_sigquit;
    sigemptyset(&sa_sigquit.sa_mask);
    sa_sigquit.sa_flags = 0;
    sigaction(SIGQUIT, &sa_sigquit, NULL);

    printf("press Ctrl+C for beep, Ctrl+\\ to quit.\n");

    while (1) {
        pause();
    }

    return 0;
}

