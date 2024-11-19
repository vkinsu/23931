// #include <ctype.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <termios.h>
// #include <unistd.h>

// #define BELL write(1, "\a", 1)
// #define BACKSPACE write(1, "\b \b", 3)
// #define MAX_LINE_LENGTH 40

// int main() {
//     struct termios tty_orig, tty;
//     char line[512];
//     int pos = 0, tempPos, oldPos;
//     char tempChar;

//     if (isatty(0) != 1) {
//         fprintf(stderr, "ERROR: stdin does not represent a terminal\n");
//         exit(1);
//     }

//     if (tcgetattr(0, &tty) == -1) {
//         fprintf(stderr, "ERROR: failed to get terminal settings\n");
//         exit(2);
//     }

//     tty_orig = tty;

//     tty.c_lflag &= ~(ICANON | ISIG | ECHO);
//     tty.c_cc[VMIN] = 1;
//     tty.c_cc[VTIME] = 1;

//     if (tcsetattr(0, TCSANOW, &tty) == -1) {
//         fprintf(stderr, "ERROR: failed to set terminal settings\n");
//         exit(3);
//     }

//     while (read(0, &tempChar, 1) > 0) {
//         if (tempChar == tty.c_cc[VEOF] && pos == 0) break;

//         else if (tempChar == tty.c_cc[VERASE] && pos > 0) {
//             BACKSPACE;
//             pos--;
//         }

//         else if (tempChar == tty.c_cc[VKILL] && pos > 0) {
//             while (pos > 0) {
//                 BACKSPACE;
//                 pos--;
//             }
//         }

//         else if (tempChar == tty.c_cc[VWERASE] && pos > 0) {
//             while (pos > 0 && line[pos - 1] == ' ') {
//                 BACKSPACE;
//                 pos--;
//             }
//             while (pos > 0 && line[pos - 1] != ' ') {
//                 BACKSPACE;
//                 pos--;
//             }
//         }

//         else if (tempChar == '\n') {
//             pos = 0;
//             write(1, &tempChar, 1);
//         }

//         else if (!isprint(tempChar)) BELL;

//         else {
//             write(1, &tempChar, 1);
//             line[pos++] = tempChar;
//         }

//         if (pos >= MAX_LINE_LENGTH && tempChar != ' ') {
//             oldPos = pos;
//             while (pos > 0 && line[pos - 1] != ' ') pos--;

//             if (pos > 0) {
//                 tempPos = 0;
//                 for (int i = pos; i < oldPos; i++) {
//                     BACKSPACE;
//                     line[tempPos++] = line[i];
//                 }
//                 pos = 0;
//                 write(1, "\n", 1);
//                 for (; pos < tempPos; pos++) {
//                     write(1, &line[pos], 1);
//                 }
//             } else {
//                 write(1, "\n", 1);
//             }
//         }
//     }

//     tcsetattr(0, TCSANOW, &tty_orig);
//     return 0;
// }
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <sys/termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>

#define BELL write(1, "\a", 1)
#define BACKSPASE write(1, "\b \b", 3)
#define MAX_LINE_LENGHT 40

const char *red = "\x1b[31m";
const char *reset = "\x1b[0m";
const char *green = "\x1b[32m";
const char *purple = "\x1b[35m";


int main() {
    struct termios tty_orig, tty;
    char line [512];
    int pos = 0,tempPos, oldPos;
    char tempChar;

    if (isatty(0)!=1) {
        printf("%sERROR : stdin does not represent a terminal\n%s", red, reset);
        exit(1);
    }

    if (tcgetattr(0,&tty) == -1) {
        printf("%sERROR : failed to get terminal settings\n%s", red, reset);
        exit(2);
    }

    tty_orig = tty;

    tty.c_lflag &= ~(ICANON | ISIG | ECHO);
    tty.c_cc[VMIN] = 1;
    tty.c_cc[VTIME] = 1;

    if(tcsetattr(0, TCSANOW, &tty)==-1) {
        printf("%sERROR : failed to set terminal settings\n%s", red, reset);
        exit(3);
    }

    while(read(0,&tempChar,1)>0) {

        if (tempChar == tty.c_cc[VEOF] && pos == 0) break;

        else if(tempChar == tty.c_cc[VERASE] && pos>0) {
            BACKSPASE;
            pos--;
        }

        else if(tempChar == tty.c_cc[VKILL] && pos>0) {
            while(pos>0) {
                BACKSPASE;
                pos--;
            }
        }

        else if(tempChar == tty.c_cc[VWERASE] && pos>0) {
            while(pos>0 && line[pos-1]==32) {
                BACKSPASE;
                pos--;
            }
            while(pos>0 && line[pos-1]!=32) {
                BACKSPASE;
                pos--;
            }
        }

        else if(tempChar == '\n') {
            pos=0;
            write(1,&tempChar,1);
        }

        else if(!isprint(tempChar)) BELL;

        else {
            write(1,&tempChar,1);
            line[pos++] = tempChar;
        }

        if (pos >= MAX_LINE_LENGHT && tempChar!=' ') {
            oldPos = pos;
            while(pos>0 && line[pos-1]!=' ') pos--;

            if(pos>0) {
                tempPos = 0;
                for (int i = pos; i<oldPos; i++) {
                    BACKSPASE;
                    line[tempPos++] = line[i];
                }
                pos = 0;
                write(1, "\n",1);
                for (; pos<tempPos;pos++) {
                    write(1,&line[pos],1);
                }
            }
            else {
                write(1, "\n", 1);
            }
        }
    }

    tcsetattr(0,TCSANOW,&tty_orig);
    return 0;
}