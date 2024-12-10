#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LENGTH 40

// [32-126]

void disable_echo()
{
    struct termios t;
    tcgetattr(STDIN_FILENO, &t);
    t.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSANOW, &t);

    // setvbuf(stdout, NULL, _IONBF, 0); // Disable buffering for stdout

    printf("\r");
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

        if ((c == EOF || c == -1 || c == '0') && pos == 0) // c == EOF && pos == 0
        {
            // CTRL-D at the beginning of the line, exit program
            printf("\nexiting...\n");
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
        else if (c < 32 || c > 126) // Non-printable character (CTRL-G: Beep)
        {
            putchar(7);     // ASCII 7 is the beep sound (BEL)
            fflush(stdout); // Flush immediately
        }
        else if (isprint(c))
        {
            if (pos < MAX_LINE_LENGTH)
            {
                line[pos++] = c;
                putchar(c);     // Display the character
                fflush(stdout); // Flush immediately
            }
            else
            {
                // Coppy overflow into (another) buffer
                int posOverFlow = 0;
                char overFlowBuffer[MAX_LINE_LENGTH];

                pos--;
                while (pos > 0 && line[pos - 1] != ' ')
                {
                    overFlowBuffer[posOverFlow++] = line[pos];
                    printf("\b \b");
                }

                // Print the new line, starting from the overflow from the buffer
                printf("\n");
                pos = 0;

                for (int i = 0; i < posOverFlow; i++)
                {
                    putchar(overFlowBuffer[i]);
                    line[pos++] = overFlowBuffer[i];
                }
                fflush(stdout); // Flush immediately
            }
        }
    }
    printf("\n");
}

int main()
{
    disable_echo(); // Turn off canonical mode and echo

    printf("Enter text (Ctrl-D to exit):\n");
    for (int i = 0; i < MAX_LINE_LENGTH; i++)
    {
        printf("-");
    }
    printf("|\n");
    handle_input();

    enable_echo(); // Restore canonical mode and echo
    return 0;
}
