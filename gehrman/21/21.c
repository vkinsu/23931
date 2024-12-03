#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

int sigint_count = 0;

void handle_sigint(int signum)
{
    sigint_count++;
    printf("\a\n");
}

void handle_sigquit(int signum)
{
    printf("\nSIGINT was triggered %d time(s).\n", sigint_count);
    exit(0);
}

int main()
{
    // // https://pubs.opengroup.org/onlinepubs/009695399/functions/sigaction.html
    // struct sigaction sa_int;
    // sa_int.sa_handler = handle_sigint;
    // sigemptyset(&sa_int.sa_mask);
    // sa_int.sa_flags = SA_RESTART;

    // struct sigaction sa_quit;
    // sa_quit.sa_handler = handle_sigquit;
    // sigemptyset(&sa_quit.sa_mask);
    // sa_quit.sa_flags = SA_RESTART;

    signal(SIGINT, handle_sigint);
    signal(SIGQUIT, handle_sigquit);

    printf("Press CTRL-C to emit a beep. Press CTRL-\\ (SIGQUIT) to exit.\n");

    while (1)
    {
        pause();
    }

    return 0;
}