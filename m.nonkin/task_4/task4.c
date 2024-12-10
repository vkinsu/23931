#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 1024 

typedef struct Node {
    char* data;
    struct Node* next;
} Node;

// add_to_list
void append(Node** head, char* str) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    if (!newNode) {
        perror("Can't set memory");
        exit(EXIT_FAILURE);
    }
    newNode->data = str;
    newNode->next = NULL;
    
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node* temp = *head;
        while (temp->next != NULL) {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}


void freeList(Node* head) {
    Node* temp;
    while (head != NULL) {
        temp = head;
        head = head->next;
        free(temp->data);
        free(temp);
    }
}

int main() {
    Node* head = NULL;
    char buffer[MAX_LENGTH];

    printf("\nEnter strings:\n");
    while (1) {
        if (fgets(buffer, MAX_LENGTH, stdin) == NULL) {
            perror("Input err");
            freeList(head);
            exit(EXIT_FAILURE);
        }

        // end of enterin
        if (buffer[0] == '.') {
            break;
        }

        // - symb of new str
        size_t len = strlen(buffer);
        if (len > 0 && buffer[len - 1] == '\n') {
            buffer[len - 1] = '\0';
        }

        // mem for new str
        char* str = (char*)malloc(len + 1);

        strcpy(str, buffer);
        append(&head, str);
    }

    printf("\nEntered strings:\n");
    Node* current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }

    freeList(head);

    return 0;
}
