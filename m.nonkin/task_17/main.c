#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#define LINE_LEN 40
#define DELETE_SYMBOL "\33[D\33[K"
#define DELETE_ALL_SYMBOLS_IN_STRING "\33[2K\r"

struct termios terminal, raw_mode;
void reset_terminal_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal);
}

void setup_raw_mode() {
    tcgetattr(STDIN_FILENO, &terminal);
    raw_mode = terminal;
    raw_mode.c_lflag &= ~(ECHO | ICANON);
    raw_mode.c_cc[VMIN] = 1;
    raw_mode.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw_mode);
    atexit(reset_terminal_mode);
}

void delete_last_word(char *line, int *length) {
    while (*length > 0 && isspace(line[*length - 1])) {
        write(STDOUT_FILENO, DELETE_SYMBOL, strlen(DELETE_SYMBOL));
        (*length)--;
    }
    while (*length > 0 && !isspace(line[*length - 1])) {
        write(STDOUT_FILENO, DELETE_SYMBOL, strlen(DELETE_SYMBOL));
        (*length)--;
    }
    line[*length] = '\0';
}

int main() {
    setup_raw_mode();

    char line[LINE_LEN + 1] = {0};
    int length = 0;

    printf("Custom terminal input. Press CTRL-D at the beginning of a line to exit.\n");

    char c;
    while (read(STDIN_FILENO, &c, 1) > 0) {
        if (!isprint(c)) {
            switch (c) {
                case 0x04: // CTRL-D
                    if (length == 0) return 0;
                    break;

                case 0x08: // Backspace
                case 0x7F: // Delete
                    if (length > 0) {
                        write(STDOUT_FILENO, DELETE_SYMBOL, strlen(DELETE_SYMBOL));
                        line[--length] = '\0';
                    }
                    break;

                case 0x17: // CTRL-W
                    delete_last_word(line, &length);
                    break;

                case 0x15: // CTRL-U
                    write(STDOUT_FILENO, DELETE_ALL_SYMBOLS_IN_STRING, strlen(DELETE_ALL_SYMBOLS_IN_STRING));
                    line[0] = '\0';
                    length = 0;
                    break;

                default: 
                    write(STDOUT_FILENO, "\x07", 1);
                    break;
            }
        } else {
            if (length < LINE_LEN) {
                line[length++] = c;
                write(STDOUT_FILENO, &c, 1);
            } else {
                write(STDOUT_FILENO, "\n", 1);
                line[0] = c;
                length = 1;
                write(STDOUT_FILENO, &c, 1);
            }
        }
    }

    return 0;
}

