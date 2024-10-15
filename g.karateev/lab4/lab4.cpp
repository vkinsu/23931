#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LEN 100  // Максимальная длина строки

// Структура узла списка
struct Node {
    char *data;
    struct Node *next;
};

// Функция для добавления строки в конец списка
struct Node* add_to_list(struct Node *head, const char *str) {
    // Выделяем память под новый узел
    struct Node *new_node = (struct Node*)malloc(sizeof(struct Node));
    if (new_node == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Выделяем память под строку
    new_node->data = (char*)malloc(strlen(str) + 1);
    if (new_node->data == NULL) {
        perror("Failed to allocate memory for string");
        exit(EXIT_FAILURE);
    }

    strcpy(new_node->data, str);
    new_node->next = NULL;

    // Если список пуст, новый узел становится первым элементом
    if (head == NULL) {
        return new_node;
    }

    // Иначе находим последний узел и добавляем новый узел в конец
    struct Node *current = head;
    while (current->next != NULL) {
        current = current->next;
    }
    current->next = new_node;

    return head;
}

// Функция для вывода всех строк в списке
void print_list(struct Node *head) {
    struct Node *current = head;
    while (current != NULL) {
        printf("%s", current->data);  // Строка уже содержит символ новой строки
        current = current->next;
    }
}

// Функция для освобождения памяти списка
void free_list(struct Node *head) {
    struct Node *current = head;
    struct Node *next;
    while (current != NULL) {
        next = current->next;
        free(current->data);  // Освобождаем память для строки
        free(current);  // Освобождаем память для узла
        current = next;
    }
}

int main() {
    struct Node *head = NULL;  // Инициализируем пустой список
    char input[MAX_LEN];  // Буфер для ввода строки

    printf("Enter strings ('.' to stop):\n");
    while (1) {
        fgets(input, MAX_LEN, stdin);  // Чтение строки
        if (input[0] == '.') {
            break;  // Остановка ввода
        }
        head = add_to_list(head, input);  // Добавляем строку в конец списка
    }

    printf("\nList of entered strings (in order of input):\n");
    print_list(head);  // Выводим строки

    free_list(head);  // Освобождаем память списка
    return 0;
}
