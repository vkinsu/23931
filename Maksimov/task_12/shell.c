#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include "shell.h"

char *infile, *outfile, *appfile;
struct command cmds[MAXCMDS];
char bkgrnd;

int main(int argc, char *argv[]) {
    register int i;
    char line[1024];      /*  allow large command lines  */
    int ncmds;
    char prompt[50];      /* shell prompt */

    /* PLACE SIGNAL CODE HERE */

    sprintf(prompt,"[%s] ", argv[0]);

    while (promptline(prompt, line, sizeof(line)) > 0) {    /*until eof  */
        if ((ncmds = parseline(line)) <= 0) continue;   /* read next line */
        #ifdef DEBUG {
            for (int i = 0; i < ncmds; i++) {
                for (int j = 0; cmds[i].cmdargs[j] != (char *) NULL; j++)
                    fprintf(stderr, "cmd[%d].cmdargs[%d] = %s\n",
                i, j, cmds[i].cmdargs[j]);
                fprintf(stderr, "cmds[%d].cmdflag = %o\n", i, cmds[i].cmdflag);
            }
        }
        #endif

        for (int i = 0; i < ncmds; i++) {

            /*  FORK AND EXECUTE  */

        }

    }  /* close while */
    return 0;
}

/* PLACE SIGNAL CODE HERE */