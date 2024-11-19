#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    int fd;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if ((fd = open(argv[1], O_RDONLY)) == -1) {
        perror("Failed to open file");
        exit(EXIT_FAILURE);
    }
    
   
    char symbol;
    int len_of_str[500];
    len_of_str[0] = 0;
    int line_count = 1; // lines
    int byte_count = 0; // bytes

    while (read(fd, &symbol, 1) == 1) {
        byte_count++;
        if (symbol == '\n') {
            len_of_str[line_count++] = byte_count;
        }
    }

    // исключение для ластовой строки - создаю экстра строку
    if (byte_count > len_of_str[line_count - 1]) {
        len_of_str[line_count++] = byte_count;
    }

    int line_num;
    while (1) {
        printf("Enter line number (0 to quit): ");
        if (scanf("%d", &line_num) != 1){
		printf("Input error\n");
		break;
	}

        if (line_num == 0) {
            break;
        }

        if (line_num >= line_count) {
            fprintf(stderr, "Line number out of range\n");
            continue;
        }

        int start_position = (line_num > 1) ? len_of_str[line_num - 1] : 0;
        int end_position = len_of_str[line_num];
        int line_length = end_position - start_position;
        
        lseek(fd, start_position, SEEK_SET);

        char buffer[1024];
        if (read(fd, buffer, line_length) == line_length) {
            write(STDOUT_FILENO, buffer, line_length);
            printf("\n");
        } else {
            perror("Failed to read line");
        }
    }

    close(fd);
    return 0;
}
