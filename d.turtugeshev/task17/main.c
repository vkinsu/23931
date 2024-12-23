#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <fcntl.h>

#define LINE_CAP 40

struct termios original_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1) {
       perror("tcgetattr");
       exit(EXIT_FAILURE);
    }
    atexit(disable_raw_mode);
    struct termios raw = original_termios;
    raw.c_lflag &= ~(ICANON | ECHO); 
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
       perror("tcsetattr");
       exit(EXIT_FAILURE);
    }
}

void beep() {
    if (write(STDOUT_FILENO, "\a", 1) == -1) {
       perror("write");
       exit(EXIT_FAILURE);
    }
}

int main() {
    enable_raw_mode();

    char line[LINE_CAP + 1] = {0};
    int position = 0;
    char c;

    const char ctrl_d = 4;
    const char ctrl_w = 23;
    const char erase = 127;
    const char kill = 21;

    for (;;) {
        if (read(STDIN_FILENO, &c, 1) == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        if (c == ctrl_d && position == 0) {
            break;
        }
        if (c == ctrl_w) {
            while (position > 0 && line[position - 1] == ' ') {
                position--;
            }
            while (position > 0 && line[position - 1] != ' ') {
                position--;
            }
            while (position < LINE_CAP && line[position] == ' ') {
                position++;
            }
            line[position] = '\0';
            char buffer[LINE_CAP + 5];
            int len = snprintf(buffer, sizeof(buffer), "\r\033[K%s", line);
            if (len > 0) {
                if (write(STDOUT_FILENO, buffer, len) == -1) {
                    perror("write");
                    exit(EXIT_FAILURE);
                }
            }
            continue;
        }
        if (c == erase) {
            if (position > 0) {
                position--;
                line[position] = '\0';
                char buffer[LINE_CAP + 5];
                int len = snprintf(buffer, sizeof(buffer), "\r\033[K%s", line);
                if (len > 0) {
                    if (write(STDOUT_FILENO, buffer, len) == -1) {
                        perror("write");
                        exit(EXIT_FAILURE);
                    }
                }
            }
            continue;
        }
        if (c == kill) {
            position = 0;
            line[position] = '\0';
            const char *clear_line = "\r\033[K\r";
            if (write(STDOUT_FILENO, clear_line, strlen(clear_line)) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            continue;
        }
        if (c == '\n' || c == '\r') {  
            line[position] = '\0';  
            const char *newline = "\n";  
            if (write(STDOUT_FILENO, newline, 1) == -1) {
                perror("write");
              exit(EXIT_FAILURE);
            }
            if (write(STDOUT_FILENO, line, position) == -1) {
                perror("write");
              exit(EXIT_FAILURE);
            }
            if (write(STDOUT_FILENO, newline, 1) == -1) {
                perror("write");
              exit(EXIT_FAILURE);
            }
            memset(line, 0, sizeof(line));  
            position = 0;  
            continue;
        }
        if (position < LINE_CAP && c >= 32 && c <= 126) {
            line[position++] = c;
            line[position] = '\0';
            if (write(STDOUT_FILENO, &c, 1) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
        } else if (c < 32 || c > 126) {
            beep();
        }
        if (position >= LINE_CAP) {
            line[position] = '\0';  
            const char *newline = "\n";  
            if (write(STDOUT_FILENO, newline, 1) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            if (write(STDOUT_FILENO, line, position) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            if (write(STDOUT_FILENO, newline, 1) == -1) {
                perror("write");
                exit(EXIT_FAILURE);
            }
            memset(line, 0, sizeof(line));
            position = 0;
        }
    }

}