#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t beep_count = 0;

// Обработчик SIGINT
void handle_sigint(int sig) {
    (void)sig; 
    beep_count++;
    printf("\a");
    fflush(stdout);
}


void handle_sigquit(int sig) {
    (void)sig;
    printf("\nProgram received SIGQUIT. Total beeps: %d\n", beep_count);
    exit(0);
}

int main() {

    struct sigaction sa_int = {0};
    sa_int.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa_int, NULL);

    struct sigaction sa_quit = {0};
    sa_quit.sa_handler = handle_sigquit;
    sigaction(SIGQUIT, &sa_quit, NULL);

    printf("Beep program started. Press CTRL-C to beep, CTRL-\\ to quit.\n");

    while (1) {
        pause();
    }

    return 0;
}
