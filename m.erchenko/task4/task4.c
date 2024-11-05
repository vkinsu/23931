#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Node {
    char *line;
    struct Node *next;
};

// Функция добавления узла в конец списка
void addNode(struct Node **head, const char *line) {
    struct Node *newNode = (struct Node *)malloc(sizeof(struct Node));
    if (newNode == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для нового узла.\n");
        exit(1);
    }

    newNode->line = (char *)malloc(strlen(line) + 1);
    if (newNode->line == NULL) {
        fprintf(stderr, "Ошибка выделения памяти для строки.\n");
        free(newNode);
        exit(1);
    }

    strcpy(newNode->line, line);
    newNode->next = NULL; // Новый узел будет последним

    if (*head == NULL) {
        // Если список пуст, новый узел становится головой
        *head = newNode;
    } else {
        // Иначе находим конец списка и добавляем новый узел
        struct Node *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

// Функция вывода всех строк из списка
void show(const struct Node *head) {
    const struct Node *current = head;
    while (current != NULL) {
        printf("%s", current->line);
        current = current->next;
    }
}

// Функция освобождения памяти для списка
void deleteList(struct Node *head) { // Изменил название функции, чтобы не конфликтовало с delete в C++
    struct Node *current = head;
    while (current != NULL) {
        struct Node *next = current->next;
        free(current->line);
        free(current);
        current = next;
    }
}

int main() {
    struct Node *head = NULL;
    char buffer[1000];
    while (1) {
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break; // Завершить ввод при ошибке или EOF
        }
        if (buffer[0] == '.') {
            break;
        }
        addNode(&head, buffer);
    }
    show(head);
    deleteList(head);
    return 0;
}
