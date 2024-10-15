#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>

#define BUFFER_SIZE 128

int timeout = 0;

void handleAlarm(int var){
	timeout = 1;
}

int main(const int argc, const char** argv){
	if(argc == 1){
		printf("Specify a filename");
		exit(-1);
	}

	int f = open(argv[1], O_RDONLY);
	if(f == -1){
		printf("Couldn't open a file!");
		exit(1);
	}


	int data_size = 0;
	int data_capacity = BUFFER_SIZE * sizeof(char);
	char* data = (char*)malloc(data_capacity);

	int bytes_read;
	while((bytes_read = read(f, data, BUFFER_SIZE)) > 0){
		data = (char*)realloc(data, data_capacity + BUFFER_SIZE * sizeof(char));
		data_capacity += BUFFER_SIZE * sizeof(char);
		data_size += bytes_read;
	}

	int line_starts_array_capacity = BUFFER_SIZE * sizeof(int);
	int line_starts_array_size = 1;
	int* line_starts_array = (int*)malloc(line_starts_array_capacity);
	line_starts_array[0] = 0;

	for(int i = 0; i < data_size; i++){
		if(line_starts_array_capacity == line_starts_array_size){
			line_starts_array = (int*)realloc(line_starts_array, line_starts_array_capacity * 2);
			line_starts_array_capacity *= 2;
		}

		if(data[i] == '\n') line_starts_array[line_starts_array_size++] = i;
	}
	while(1){
		timeout = 0;
		signal(SIGALRM, handleAlarm);
		alarm(5);

		int line_num;
		printf("Enter line number (5 sec timer): ");
		if(scanf("%d", &line_num) == 1){
			alarm(0);
		}

		if(timeout){
			printf("Timeout occured!\n");
			printf("%s", data);
		}
		if(!timeout){
			if(line_num == 0){
				close(f);
				exit(0);
			}
			if(line_num >= line_starts_array_size) printf("Wrong line number!\n");
			else{
				int line_start = lseek(f, line_starts_array[line_num - 1], SEEK_SET);
				if(data[line_start] == '\n') lseek(f, ++line_start, SEEK_SET);
				int line_end = line_starts_array[line_num];
				char* line = (char*)malloc(sizeof(char) * (line_end - line_start));
				bytes_read = read(f, line, line_end - line_start);
				printf("line = %s\n", line);
				free(line);
			}
		}
	}
	free(data);
	free(line_starts_array);
	close(f);
	return 0;
}
