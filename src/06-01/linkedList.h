#pragma once

#include <stdbool.h>

struct ListNode {
    char* code;
    char* name;
    struct ListNode* next;
};

char* findInList(struct ListNode* head, char* code);
bool insertToList(struct ListNode** head, char* code, char* name);
bool deleteFromList(struct ListNode** head, char* code);
void freeList(struct ListNode* head);
