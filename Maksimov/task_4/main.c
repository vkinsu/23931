#include <stdio.h>
#include <stdlib.h>


void resize_array(int* size, int* capacity, char** arr, char* str){
	if(*size < *capacity){
		arr[*size] = str;
		(*size)++;
	}
	else{
		*capacity *= 2;
		arr = (char**)realloc(arr, sizeof(char**) * *capacity);
		arr[*size] = str;
		*size++;
	}

}


int main(){
	int size = 0;
	int capacity = 50;
	int str_size = 256;
	char** str_ptrs = (char**)malloc(sizeof(char*) * capacity);
	char* str = (char*)malloc(sizeof(char) * 256);
	fgets(str, str_size, stdin);
	resize_array(&size, &capacity, str_ptrs, str);
	//str_ptrs[size] = str;
	//size++;
	//printf("str = %s", str);
	while(str[0] != '.'){
		str = (char*)malloc(sizeof(char) * 256);
		fgets(str, str_size, stdin);
		resize_array(&size, &capacity, str_ptrs, str);
		//str_ptrs[size] = str;
		//size++;
		//printf("str = %s", str);
	}
	for(int i = 0; i < size; i++){
		printf("str %d = %s", i, str_ptrs[i]);
	}
	return 0;
}

