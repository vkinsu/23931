#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#define LINE_LEN 40
#define ERASE   127
#define KILL    21
#define CTRL_W  23
#define CTRL_D   4
#define BELL     7

static struct termios oldt;

void enable_raw_mode() {
    struct termios newt;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    newt.c_cc[VMIN] = 1;
    newt.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
}

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
}

void print_bell() {
    write(STDOUT_FILENO, "\a", 1);
}

void erase_chars_from_screen(int count) {
    for (int i = 0; i < count; i++)
        write(STDOUT_FILENO, "\b \b", 3);
}

int is_printable_char(unsigned char c) {
    return (c >= 32 && c <= 126);
}

int is_space(unsigned char c) {
    return isspace(c);
}

void delete_last_word(char *line, int *col) {
    int len = (int)strlen(line);
    if (len == 0) return;
    int i = len - 1;
    while (i >= 0 && is_space((unsigned char)line[i])) i--;
    while (i >= 0 && !is_space((unsigned char)line[i])) i--;
    int new_len = i+1;
    int to_delete = len - new_len;
    erase_chars_from_screen(to_delete);
    line[new_len] = '\0';
    *col = new_len;
}

void delete_char(char *line, int *col) {
    int len = (int)strlen(line);
    if (len > 0) {
        erase_chars_from_screen(1);
        line[len-1] = '\0';
        (*col)--;
    } else {
        print_bell();
    }
}

void kill_line(char *line, int *col) {
    int len = (int)strlen(line);
    erase_chars_from_screen(len);
    line[0] = '\0';
    *col = 0;
}

void new_line(char *line, int *col) {
    write(STDOUT_FILENO, "\n", 1);
    line[0] = '\0';
    *col = 0;
}

void print_string(const char *str, char *line, int *col) {
    int len = (int)strlen(str);
    for (int i = 0; i < len; i++) {
        if (*col == LINE_LEN) {
            new_line(line, col);
        }
        write(STDOUT_FILENO, &str[i], 1);
        line[*col] = str[i];
        line[*col+1] = '\0';
        (*col)++;
    }
}

int main() {
    enable_raw_mode();
    atexit(disable_raw_mode);

    char line[LINE_LEN+1]; 
    line[0] = '\0';
    int col = 0;
    int word_start = -1;
    int running = 1;

    while (running) {
        unsigned char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) {
            break;
        }

        if (c == CTRL_D) {
            if (strlen(line) == 0) {
                running = 0;
            } else {
                print_bell();
            }
            continue;
        }

        if (c == ERASE) {
            if (strlen(line) > 0) {
                char last_char = line[strlen(line)-1];
                delete_char(line, &col);
                if (!is_space((unsigned char)last_char) && word_start >= 0) {
                    if (word_start >= (int)strlen(line)) {
                        word_start = -1;
                    }
                }
            } else {
                print_bell();
            }
            continue;
        }

        if (c == KILL) {
            kill_line(line, &col);
            word_start = -1;
            continue;
        }

        if (c == CTRL_W) {
            delete_last_word(line, &col);
            {
                int len = (int)strlen(line);
                int i = len - 1;
                while (i >= 0 && is_space((unsigned char)line[i])) i--;
                if (i < 0) {
                    word_start = -1;
                } else {
                    while (i >= 0 && !is_space((unsigned char)line[i])) i--;
                    word_start = i + 1;
                }
            }
            continue;
        }

        if ((!is_printable_char(c)) && c != ' ' && c != '\t' && c != '\n') {
            if (c != '\r' && c != '\n' && c != '\t' && c != ' ') {
                print_bell();
                continue;
            }
        }

        int is_delimiter = is_space(c);

        if (is_delimiter) {
            if (word_start >= 0) {
                int word_length = (int)strlen(line) - word_start;
                int end_pos = word_start + word_length;
                if (word_start + word_length > LINE_LEN) {
                    char temp_word[LINE_LEN+1];
                    memcpy(temp_word, &line[word_start], word_length);
                    temp_word[word_length] = '\0';
                    erase_chars_from_screen(word_length);
                    line[word_start] = '\0';
                    col = word_start;
                    new_line(line, &col);
                    print_string(temp_word, line, &col);
                }
                word_start = -1;
            }

            if (c != '\n' && c != '\r') {
                if (col == LINE_LEN) {
                    new_line(line, &col);
                }
                write(STDOUT_FILENO, &c, 1);
                int len = (int)strlen(line);
                line[len] = c;
                line[len+1] = '\0';
                col++;
            }

        } else {
            if (word_start < 0) {
                word_start = col;
            }

            if (!is_printable_char(c)) {
                print_bell();
                continue;
            }

            if (col == LINE_LEN) {
                new_line(line, &col);
                word_start = 0; 
            }

            int len = (int)strlen(line);
            if (len < LINE_LEN) {
                write(STDOUT_FILENO, &c, 1);
                line[len] = c;
                line[len+1] = '\0';
                col++;
            } else {
                print_bell();
            }
        }
    }

    disable_raw_mode();
    write(STDOUT_FILENO, "\n", 1);

    return 0;
}
