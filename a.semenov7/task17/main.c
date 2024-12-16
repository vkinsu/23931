#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

void disable_echo_and_canonical(struct termios *old_tio) {
    struct termios new_tio;
    tcgetattr(STDIN_FILENO, old_tio);
    new_tio = *old_tio;
    new_tio.c_lflag &= ~ICANON; 
    new_tio.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);
}

void restore_terminal(struct termios *old_tio) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_tio);
}

int main() {
    struct termios old_tio;
    disable_echo_and_canonical(&old_tio);

    char line[40 + 1] = {0}; 
    int pos = 0;

    while (1) {
        char c;
        read(STDIN_FILENO, &c, 1);

        if (c == 4 && pos == 0) //Ctrl-D
            break;
        else if (c == 127) //Backspace
        {
            if (pos > 0) {
                (pos)--;
                line[pos] = '\0';
                write(STDOUT_FILENO, "\b \b", 3);
            } else {
                write(STDOUT_FILENO, "\a", 1);
            }
        }
        else if (c == 21)
        { //Ctrl-U
            while (pos > 0) {
                (pos)--;
                write(STDOUT_FILENO, "\b \b", 3);
            }
            line[0] = '\0';
        }
        else if (c == 23) //Ctrl-W
        {
            while (pos > 0 && line[pos - 1] == ' ') {
                if (pos > 0) {
                    (pos)--;
                    line[pos] = '\0';
                    write(STDOUT_FILENO, "\b \b", 3);
                } else {
                    write(STDOUT_FILENO, "\a", 1);
                }
            }
            while (pos > 0 && line[pos - 1] != ' ') {
                if (pos > 0) {
                    (pos)--;
                    line[pos] = '\0';
                    write(STDOUT_FILENO, "\b \b", 3);
                } else {
                    write(STDOUT_FILENO, "\a", 1);
                }
            }
        }
        else if (c >= 32 && c <= 126) { 
            if (pos < 40) {
                line[pos++] = c;
                write(STDOUT_FILENO, &c, 1);
                if (pos == 40) {
                    write(STDOUT_FILENO, "\n", 1);
                    pos = 0;
                }
            } 
            else 
                write(STDOUT_FILENO, "\a", 1);
        }
        else 
            write(STDOUT_FILENO, "\a", 1);
    }

    restore_terminal(&old_tio);
    write(STDOUT_FILENO, "\n", 1);
    return 0;
}
