#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <termios.h>
#include <unistd.h>

#define ERASE write(1, "\b \b", 4)
#define   BEL "\07"  

int main() {
    struct termios settings;
    struct termios saved_settings;
    tcgetattr(0, &settings);
    //memcpy(&saved_settings, &settings, sizeof(settings));
    saved_settings = settings;
    settings.c_lflag &= ~(ISIG | ICANON | ECHO);
    settings.c_cc[VMIN] = 1;
    settings.c_cc[VTIME] = 1;
    tcsetattr(0, TCSANOW, &settings);

    char line[41];
    int pos = 0, newpos, savpos, i;
    while (read(0, &line[pos], 1) == 1) {
        if (pos > 0 && line[pos] == settings.c_cc[VERASE]) {
            ERASE;
            --pos;
        }
        else if (line[pos] == settings.c_cc[VKILL]) {
            while (pos > 0) {
                ERASE;
                --pos;
            }
        }
        else if (line[pos] == settings.c_cc[VWERASE]) {
            while (pos > 0 && !isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
            while (pos > 0 && isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
        }
        else if(line[pos]=='\n'){
            write(1, &line[pos], 1);
            pos = 0;
        }
        else if (pos == 0 && line[pos] == CEOT) {
            break;
        }
        else if (!isprint(line[pos]))
            write(1, BEL, 1);
        else if (pos == 40) {
            while (pos > 0 && !isspace(line[pos - 1])) {
                ERASE;
                --pos;
            }
            write(1, "\n", 1);
            int k = 0;
            int i;
            for (i = pos; i < 40; ++i) {
                write(1, &line[i], 1);
                ++k;
            }
            pos = k;
        }
        else {
            write(1, &line[pos], 1);
            ++pos;
        }
        if (pos >= 40 && !isspace(line[pos])) {
            savpos = pos;
            /* find begining of last word */
            while (pos > 0 && !isspace(line[pos-1]))
                --pos;
            if (pos > 0) {
                newpos = 0;
                /* copy last word to beginning of line */
                for (i = pos; i < savpos; i++) {
                    ERASE;
                    line[newpos++] = line[i];
                }
                pos = newpos;
                /* display word at beginning of line */
                write(1, "\n", 1);
                for (i = 0; i < pos; i++)
                    write(1, &line[i], 1);
            }
            else
                write(1, "\n", 1);
          }
    }
    tcsetattr(0, TCSANOW, &saved_settings);
    return 0;
}
