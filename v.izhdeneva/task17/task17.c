#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

#define MAX_LEN 40
#define ERASE 127 
#define KILL 21
#define CTRL_W 23
#define CTRL_D 4
#define CTRL_G 7

void erase_char(char* buffer, int* len) {
    if (*len > 0) {
        (*len)--;
        printf("\b \b");
        fflush(stdout);
    }
}

void kill_line(char* buffer, int* len) {
    while (*len > 0) {
        erase_char(buffer, len);
    }
}

void erase_word(char* buffer, int* len) {
    while (*len > 0 && buffer[*len - 1] == ' ') {
        erase_char(buffer, len);
    }
    while (*len > 0 && buffer[*len - 1] != ' ') {
        erase_char(buffer, len);
    }
}

void print_char(char* buffer, int* len, char c) {
    if (*len >= MAX_LEN) {
        printf("\n");
        fflush(stdout);
        *len = 0;
    }
    buffer[(*len)++] = c;
    putchar(c);
    fflush(stdout);
}

int main() {
    struct termios old_tio;

    struct termios new_tio;
    tcgetattr(STDIN_FILENO, &old_tio); 
    new_tio = old_tio;
    new_tio.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    char buffer[MAX_LEN + 1];
    int len = 0;

    printf("Custom line editor (CTRL-D to exit)\n");

    while (1) {
        char c;
        if (read(STDIN_FILENO, &c, 1) <= 0) {
            break;
        }

        if (c == CTRL_D && len == 0) {
            printf("\nExiting...\n");
            break;
        } else if (c == ERASE) {
            erase_char(buffer, &len);
        } else if (c == KILL) {
            kill_line(buffer, &len);
        } else if (c == CTRL_W) {
            erase_word(buffer, &len);
        } else if (c >= 32 && c <= 126) {
            print_char(buffer, &len, c);
        } else {
            putchar(CTRL_G);
            fflush(stdout);
        }
    }

    tcsetattr(STDIN_FILENO, TCSANOW, &old_tio);

    exit(0);
}
