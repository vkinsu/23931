#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
 
#define MAX_LENGTH 256 
 
typedef struct Element { 
    char *content; 
    struct Element *next; 
} Element; 
 
Element* append_to_list(Element *start, const char *input) { 
    Element *new_element = (Element*)malloc(sizeof(Element)); 
    if (new_element == NULL) { 
        fprintf(stderr, "error memory allocation.n"); 
        exit(1); 
    } 
 
    new_element->content = (char*)malloc(strlen(input) + 1); 
    if (new_element->content == NULL) { 
        fprintf(stderr, "error memory allocation.n"); 
        free(new_element); 
        exit(1); 
    } 
 
    strcpy(new_element->content, input); 
    new_element->next = NULL; 
 
    if (start == NULL) { 
        return new_element; 
    } else { 
        Element *current = start; 
        while (current->next != NULL) { 
            current = current->next; 
        } 
        current->next = new_element; 
    } 
    return start; 
} 
 
void release_list(Element *start) { 
    Element *temp; 
    while (start != NULL) { 
        temp = start; 
        start = start->next; 
        free(temp->content); 
        free(temp); 
    } 
} 
 
void display_list(Element *start) { 
    Element *current = start; 
    while (current != NULL) { 
        printf("%s", current->content); 
        current = current->next; 
    } 
} 
 
int main() { 
    Element *start = NULL; 
    char input_buffer[MAX_LENGTH]; 
 
    printf("Enter strings: "); 
 
    while (1) { 
        if (fgets(input_buffer, MAX_LENGTH, stdin) == NULL) { 
            fprintf(stderr, "Error reading input.n"); 
            break; 
        } 
 
        if (input_buffer[0] == '.') { 
            break; 
        } 
 
        start = append_to_list(start, input_buffer); 
    } 
 
    printf("Your strings: "); 
    display_list(start); 
    release_list(start); 
 
    return 0; 
}
