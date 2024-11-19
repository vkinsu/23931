#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>
#define   BEL "\07"  

struct termios savtty;
int stdin_d;

void interraptCnt(int sig) {
    static int cnt = 0;
    signal(sig, SIG_IGN);
    if(sig == SIGINT){
        write(1, BEL, 1);
        cnt++;
    }
    else if(sig == SIGQUIT) {
        printf("SIGINT was received %d times.\n", cnt);
        tcsetattr(stdin_d, TCSANOW, &savtty);
        exit(0);
    }
    sigset(SIGINT, interraptCnt);
}

int main(int argc, char *argv[]) {

    while(1){
        stdin_d = fileno(stdin);
        struct termios tty;
        tcgetattr(stdin_d, &tty);
        savtty = tty;
        tty.c_lflag &= ~ECHO;
        tcsetattr(stdin_d, TCSANOW, &tty);

        sigset(SIGINT, interraptCnt);
        sigset(SIGQUIT, interraptCnt);

        tcsetattr(stdin_d, TCSANOW, &savtty);
    }

}
