#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main()
{
    char* lines[100];
    int linesCount = 0;


    char buff[100];

    while (fgets(buff, sizeof(buff), stdin))
    {
        if (buff[0] == '.')
        {
            break;
        }
        buff[strcspn(buff, "\n")] = '\0';
        size_t lineLen = strlen(buff);
        char* line = (char*)malloc((lineLen + 1) * sizeof(char));
        if (line == NULL)
        {
            perror("Failed to allocate memory");
            return 1;
        }
        strcpy(line, buff);
        
        lines[linesCount] = line;
        linesCount++;
    }

    printf("\n<*><*><*><*><*><*>\nYOU ENTERED:\n");

    for (int i = 0; i < linesCount; i++)
    {
        printf("%s\n", lines[i]);
        free(lines[i]);
    }
    return 0;
}
