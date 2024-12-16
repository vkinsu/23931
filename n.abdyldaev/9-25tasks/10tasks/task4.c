#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char* string;
    struct Node* next;
};

void add_node(struct Node** head, const char* str) {
    struct Node* new_node = (struct Node*)malloc(sizeof(struct Node));
    new_node->string = (char*)malloc(strlen(str)+1);
    strcpy(new_node->string,str);
    new_node->next = *head;
    *head = new_node;
}

int main() {
    struct Node* head=NULL;
    char input[1024];

    while (1) {
        printf("enter string or . to finish: ");
        fgets(input, sizeof(input), stdin);
        if (input[0] == '.' && input[1] == '\n') {
            break;
        }
        add_node(&head, input);
    }
    printf("\nstrings in the list:\n");
    struct Node* current = head;
    while (current != NULL) {
        printf("%s", current->string);
        current = current->next;
    }
    current = head;
    while (current != NULL) {
        struct Node* next_node = current->next;
        free(current->string);
        free(current);
        current = next_node;
    }
    return 0;
}

