#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#define ERASE 127        
#define KILL 21          // Ctrl-U
#define CTRL_W 23        // Ctrl-W
#define CTRL_D 4         // Ctrl-D
#define CTRL_G 7         // Ctrl-G
#define MAX_LINE_LEN 40  

struct termios original_termios;

void disable_raw_mode();

void enable_raw_mode() {
    struct termios raw;
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disable_raw_mode);

    raw = original_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSANOW, &raw);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &original_termios);
}

void handle_erase(char *buffer, int *length) {
    if (*length > 0) {
        (*length)--;
        buffer[*length] = '\0';
        printf("\b \b");
        fflush(stdout);
    }
}

void handle_kill(char *buffer, int *length) {
    while (*length > 0) {
        printf("\b \b");
        (*length)--;
    }
    buffer[0] = '\0';
    fflush(stdout);
}

void handle_ctrl_w(char *buffer, int *length) {
    while (*length > 0 && buffer[*length - 1] == ' ') {
        handle_erase(buffer, length);
    }
    while (*length > 0 && buffer[*length - 1] != ' ') {
        handle_erase(buffer, length);
    }
}

int main() {
    enable_raw_mode();

    char buffer[MAX_LINE_LEN + 1] = {0};
    int length = 0;

    printf("Line editor started. Use CTRL-D to exit.\n");

    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) != 1) {
            break;
        }

        if (c == CTRL_D && length == 0) {
            break;
        } else if (c == ERASE) {
            handle_erase(buffer, &length);
        } else if (c == KILL) {
            handle_kill(buffer, &length);
        } else if (c == CTRL_W) {
            handle_ctrl_w(buffer, &length);
        } else if (c < 32 || c == 127) {
            putchar(CTRL_G);
            fflush(stdout);
        } else {
            if (length < MAX_LINE_LEN) {
                buffer[length++] = c;
                buffer[length] = '\0';
                putchar(c);
                fflush(stdout);

                if (length == MAX_LINE_LEN) {
                    printf("\n");
                    fflush(stdout);
                }
            }
        }
    }

    printf("\nExiting line editor.\n");
    return 0;
}
