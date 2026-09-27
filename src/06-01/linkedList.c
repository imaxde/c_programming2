#include "linkedList.h"

#include <stdlib.h>
#include <string.h>

char* findInList(struct ListNode* head, char* code)
{
    for (struct ListNode* node = head; node != NULL; node = node->next) {
        if (strcmp(node->code, code) == 0)
            return node->name;
    }
    return NULL;
}

bool insertToList(struct ListNode** head, char* code, char* name)
{
    if (findInList(*head, code) != NULL)
        return false;

    struct ListNode* node = malloc(sizeof(struct ListNode));
    if (node == NULL)
        return false;

    node->code = strdup(code);
    node->name = strdup(name);
    if (node->code == NULL || node->name == NULL) {
        free(node->code);
        free(node->name);
        free(node);
        return false;
    }

    node->next = *head;
    *head = node;
    return true;
}

bool deleteFromList(struct ListNode** head, char* code)
{
    struct ListNode** link = head;
    while (*link != NULL && strcmp((*link)->code, code) != 0)
        link = &(*link)->next;

    if (*link == NULL)
        return false;

    struct ListNode* node = *link;
    *link = node->next;
    free(node->code);
    free(node->name);
    free(node);
    return true;
}

void freeList(struct ListNode* head)
{
    while (head != NULL) {
        struct ListNode* next = head->next;
        free(head->code);
        free(head->name);
        free(head);
        head = next;
    }
}
