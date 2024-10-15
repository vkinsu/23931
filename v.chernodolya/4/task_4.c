#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node {
    char *data;
    struct Node *next;
} Node;

void addToList(Node **head, char *str) {
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->data = (char *)malloc(strlen(str) + 1);
    strcpy(newNode->data, str);
    newNode->next = *head;
    *head = newNode;
}

void printList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}

void freeList(Node *head) {
    Node *current = head;
    Node *next;
    while (current != NULL) {
        next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}

int main() {
    Node *head = NULL;
    char buffer[256]; 

    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = 0; 

        if (strcmp(buffer, ".") == 0) {
            break;
        }

        addToList(&head, buffer);
    }

    printList(head);

    freeList(head);

    return 0;
}
