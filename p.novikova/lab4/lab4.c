#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

// Определение структуры узла списка
typedef struct Node {
    char *data;
    struct Node *next;
} Node;

// Функция для создания нового узла
Node* createNode(char *data) {
    Node *newNode = (Node*)malloc(sizeof(Node));
    if (newNode == NULL) {
        perror("Failed to allocate memory for new node");
        exit(EXIT_FAILURE);
    }
    newNode->data = strdup(data);
    if (newNode->data == NULL) {
        perror("Failed to allocate memory for string data");
        free(newNode);
        exit(EXIT_FAILURE);
    }
    newNode->next = NULL;
    return newNode;
}

// Функция для добавления узла в конец списка
void appendNode(Node **head, char *data) {
    Node *newNode = createNode(data);
    if (*head == NULL) {
        *head = newNode;
    } else {
        Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Функция для вывода всех строк из списка
void printList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        printf("%s\n", current->data);
        current = current->next;
    }
}

// Функция для освобождения памяти, выделенной под список
void freeList(Node *head) {
    Node *current = head;
    while (current != NULL) {
        Node *next = current->next;
        free(current->data);
        free(current);
        current = next;
    }
}

int main() {
    Node *head = NULL;
    char line[MAX_LINE_LENGTH];

    printf("Enter strings (end with a single '.'):\n");

    while (1) {
        if (fgets(line, MAX_LINE_LENGTH, stdin) == NULL) {
            perror("Failed to read input");
            break;
        }

        // Удаление символа новой строки, если он есть
        line[strcspn(line, "\n")] = '\0';

        // Проверка на завершение ввода
        if (line[0] == '.' && line[1] == '\0') {
            break;
        }

        // Добавление строки в список
        appendNode(&head, line);
    }

    // Вывод всех строк из списка
    printf("\nEntered strings:\n");
    printList(head);

    // Освобождение памяти, выделенной под список
    freeList(head);

    return 0;
}