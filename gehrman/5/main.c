#include <fcntl.h>      // For open()
#include <unistd.h>     // For read(), lseek(), close()
#include <sys/types.h>  // For data types like off_t
#include <sys/stat.h>   // For file mode flags (optional)
#include <stdio.h>      // For printf(), perror()


int main(int argc, char **argv)
{

    if (argc != 2)
    {
        perror("No filepath provided");
        return 1;
    }

    char *filePath = argv[1];

    int file = open(filePath, O_RDONLY);  // Open the file in read-only mode
    if (file == -1)
    {
        perror("Error opening file");
        return 1;
    }

    char buffer;
    off_t position = 0;  // Current position in the file
    off_t newline_positions[100];  // Array to store newline positions
    int newline_count = 0;  // Count of newlines found

    // Read the file byte by byte
    while (read(file, &buffer, 1) == 1)
    {
        if (buffer == '\n')
        {
            newline_positions[newline_count] = position;
            newline_count++;
        }
        position++;
    }

    // Print the newline positions
    printf("Line | Offset | Length:\n");
    for (int i = 0; i < newline_count; i++)
    {
        if (i == 0)
        {
            printf("%d \t %lld \t %lld\n", i + 1, newline_positions[i] + 1, newline_positions[i]);
        }
        else
        {
            printf("%d \t %lld \t %lld\n", i + 1, newline_positions[i] + 1, newline_positions[i] - newline_positions[i - 1] - 1);
        }
    }

    close(file);
    return 0;
}
