#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *data;        
    struct Node *next; 
};

struct Node* init_list() {
    struct Node *head = NULL;
    return head;
}

struct Node* create_node(const char *str) {
    struct Node *new_node = (struct Node*) malloc(sizeof(struct Node));

    if (!new_node) {
        printf("Memory allocation error\n");
        exit(1);
    }

    new_node->data = (char*)malloc(strlen(str) + 1);

    if (!new_node->data) {
        printf("Memory allocation error\n");
        exit(1);
    }

    strcpy(new_node->data, str);
    new_node->next = NULL;

    return new_node;
}

void append_node(struct Node **head, const char *str) {
    struct Node *new_node = create_node(str);

    if (*head == NULL) *head = new_node;
    else {
        struct Node *temp = *head;

        while (temp->next != NULL) temp = temp->next;

        temp->next = new_node;
    }
}

void print_list(struct Node *head) {
    struct Node *temp = head;

    while (temp != NULL) {
        printf("%s", temp->data);
        temp = temp->next;
    }
}

void free_list(struct Node *head) {
    struct Node *temp;

    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->data); 
        free(temp);       
    }
}
 
int main() {
    struct Node *head = init_list();

    char buffer[256];

    while (1) {
        printf("Введите строки (начните строку с точки (.) для завершения ввода):\n");

        fgets(buffer, sizeof(buffer), stdin);

        if (buffer[0] == '.') break;

        append_node(&head, buffer);
    }

    printf("\nВведенные строки: \n");
    print_list(head);

    free_list(head);

    return 0;
}
