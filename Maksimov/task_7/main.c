#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <signal.h>
#include <sys/mman.h>
#include <string.h>
#include <sys/stat.h>

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



	struct stat st;
	if(fstat(f, &st) == -1){
		printf("Couldn't get size of the file!\n");
		close(f);
		exit(1);
	}
	size_t file_size = st.st_size;

	char* data = mmap(NULL, file_size, PROT_READ, MAP_PRIVATE, f, 0);
	if(data == MAP_FAILED){
		printf("Mmap failed!\n");
		close(f);
		exit(1);
	}
	close(f);

	int line_starts_array_capacity = BUFFER_SIZE * sizeof(int);
	int line_starts_array_size = 1;
	int* line_starts_array = (int*)malloc(line_starts_array_capacity);
	line_starts_array[0] = 0;

	for(int i = 0; i < file_size; i++){
		if(line_starts_array_capacity == line_starts_array_size){
			line_starts_array = (int*)realloc(line_starts_array, line_starts_array_capacity * 2);
			line_starts_array_capacity *= 2;
		}

		if(data[i] == '\n') line_starts_array[line_starts_array_size++] = i + 1;
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
			// write(STDOUT_FILENO, data, file_size);
			printf("%s", data);
		}
		if(!timeout){
			if(line_num == 0){
				munmap(data, file_size);
				exit(0);
			}
			if(line_num >= line_starts_array_size){
				printf("Line number is out of range!\n");
			}
			else{
				int line_start = line_starts_array[line_num - 1];
				int line_end = line_starts_array[line_num] - 1;
				char* line = (char*)malloc(sizeof(char) * (line_end - line_start + 1));
				strncpy(line, data + line_start, line_end - line_start);
				line[line_end - line_start] = '\0';
				printf("line = %s\n", line);
				free(line);
			}
		}
	}
	
	free(line_starts_array);
	munmap(data, file_size);
	return 0;
}
