#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define BUFFER_SIZE 128

int main(const int argc, const char** argv){
	if(argc == 1){
		printf("Specify a filename\n");
		exit(-1);
	}

	int f = open(argv[1], O_RDONLY);
	if(f == -1){
		printf("Couldn't open a file!\n");
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

			if(data[i] == '\n'){
				// printf("found!\n");
				line_starts_array[line_starts_array_size++] = i;
			}
		}

        // printf("size = %d\n", line_starts_array_size);
        // for(int i = 0; i < line_starts_array_size; i++){
        //     printf("deb = %d\n", line_starts_array[i]);
        // }

    while(1){
        int line_num;
		printf("Enter line number: ");
		scanf("%d", &line_num);
		if(line_num == 0) exit(-1);
        if(line_num >= line_starts_array_size){
            printf("Wrong line number!\n");
            exit(-1);
        }

		if(line_num >= line_starts_array_size) exit(-1);

		int line_start = lseek(f, line_starts_array[line_num - 1], SEEK_SET);
		if(data[line_start] == '\n') lseek(f, ++line_start, SEEK_SET);
		int line_end = line_starts_array[line_num];
		char* line = (char*)malloc(sizeof(char) * (line_end - line_start + 1));
        line[line_end - line_start] = '\0';
		bytes_read = read(f, line, line_end - line_start);
        // printf("bytes read = %d\n", bytes_read);
		printf("line = %s\n", line);
		
        free(line);
	}
    free(data);
	free(line_starts_array);
	
	return 0;

}
