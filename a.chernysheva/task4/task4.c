#include <stdio.h>
#include <stdlib.h>

typedef struct List {
    char* string;
    struct List* next;
}List;

List* init(char* str, List* next) {
    List* obj = malloc(sizeof(List));
    obj->string = malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(obj->string, str);
    obj->next = next;
    return obj;
}
void delete(List* obj) {
    if (obj) {
        if (obj->string) free(obj->string);
        free(obj);
    }
}

int main() {
    char input[257];
    fgets(input, 256, stdin);
    List root;
    List last_elem;
    last_elem.next = &root;
    while (input[0] != '.') {
        List* new_elem = init(input, NULL);
        last_elem.next->next = new_elem;
        last_elem.next = new_elem;
        fgets(input, 256, stdin);
    }
    for (List* p = root.next; p != last_elem.next->next; p = p->next) {
        printf("%s\n", p->string);
    }
    return 0;
}
