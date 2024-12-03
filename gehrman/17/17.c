#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 40

void disable_echo()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void enable_echo()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag |= (ICANON | ECHO); // Restore canonical mode and echo
    tcsetattr(STDIN_FILENO, TCSANOW, &t);
}

void handle_input()
{
    char line[MAX_LINE_LENGTH + 1]; // Buffer for input
    int pos = 0;
    int c;

    while (1)
    {
        c = getchar();

        if (c == EOF && pos == 0)
        {
            // CTRL-D at the beginning of the line, exit program
            break;
        }

        if (c == 8 || c == 127)
        { // ERASE: Backspace (ASCII 8 or 127)
            if (pos > 0)
            {
                pos--;
                printf("\b \b"); // Erase the character on screen
            }
        }
        else if (c == 21)
        { // CTRL-U: KILL (Clear the whole line)
            printf("\r");
            for (int i = 0; i < pos; i++)
            {
                printf(" ");
            }
            printf("\r");
            pos = 0;
        }
        else if (c == 23)
        { // CTRL-W: Delete last word
            while (pos > 0 && line[pos - 1] == ' ')
            {
                pos--; // Remove spaces
                printf("\b \b");
            }
            while (pos > 0 && line[pos - 1] != ' ')
            {
                pos--; // Remove word characters
                printf("\b \b");
            }
        }
        else if (c == 7)
        {               // Non-printable character (CTRL-G: Beep)
            putchar(7); // ASCII 7 is the beep sound (BEL)
        }
        else if (isprint(c))
        {
            if (pos < MAX_LINE_LENGTH)
            {
                line[pos++] = c;
                putchar(c); // Display the character
            }
            else
            {
                // Line is full, wrap text
                if (pos == MAX_LINE_LENGTH)
                {
                    printf("\n");
                    pos = 0;
                }
                line[pos++] = c;
                putchar(c);
            }
        }
    }
    printf("\n");
}

int main()
{
    disable_echo(); // Turn off canonical mode and echo

    printf("Enter text (Ctrl-D to exit):\n");
    handle_input();

    enable_echo(); // Restore canonical mode and echo
    return 0;
}
