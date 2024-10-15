#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node{
    char *string;
    struct Node *next;
} Node;

Node* appendNode(char* string, Node** list)
{
    Node *cell = (Node*)malloc(sizeof(Node));
    if(cell == NULL)
    {
        printf("You broke the programm, congrats");
        exit(0);
    }
    int len = sizeof(string);
    cell->string = (char*)malloc(len+1);
    if(cell->string == NULL)
    {
        printf("You broke the programm, congrats");
        exit(0);
    }
    strcpy(cell->string, string);
    cell->next = NULL;
    if(*list == NULL)
    {
        *list = cell;
    }
    else
    {
        Node *toAppend = *list;
        while(toAppend->next != NULL)
        {
            toAppend = toAppend->next;
        }
        toAppend->next = cell;
    }
}

int main()
{
    Node* list = NULL;
    char arr[2048];

    printf("Put the strings, put the strings, PUT THE STRINGS!!!\n");
    while(1)
    {
        fgets(arr, sizeof(arr), stdin);
        if(arr[0] == '.')
            break;
        
        appendNode(arr, &list);
    }

    printf("Your strings:\n");
    while(list != NULL)
    {
        printf("%s", list->string);
        list = list->next;
    }

    while(list != NULL)
    {
        Node* nextCell = list->next;
        free(list->string);
        free(list);
        list = nextCell;
    }

    return 0;
}
