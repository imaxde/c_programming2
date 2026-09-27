#include "avlTree.h"

#include <stdlib.h>
#include <string.h>

int max(int a, int b)
{
    return a > b ? a : b;
}

int min(int a, int b)
{
    return a < b ? a : b;
}

struct Node* rotateLeft(struct Node* a)
{
    struct Node* b = a->right;
    if (b == NULL)
        exit(1);
    a->right = b->left;
    b->left = a;

    a->balance = a->balance - 1 - max(b->balance, 0);
    b->balance = b->balance - 1 + min(a->balance, 0);
    return b;
}

struct Node* rotateRight(struct Node* a)
{
    struct Node* b = a->left;
    if (b == NULL)
        exit(1);
    a->left = b->right;
    b->right = a;

    a->balance = a->balance + 1 - min(b->balance, 0);
    b->balance = b->balance + 1 + max(a->balance, 0);
    return b;
}

struct Node* balance(struct Node* node)
{
    if (node->balance == 2) {
        if (node->right == NULL)
            exit(1);
        if (node->right->balance < 0)
            node->right = rotateRight(node->right);
        return rotateLeft(node);
    }
    if (node->balance == -2) {
        if (node->left == NULL)
            exit(1);
        if (node->left->balance > 0)
            node->left = rotateLeft(node->left);
        return rotateRight(node);
    }
    return node;
}

char* findAirport(struct Node* root, char* code)
{
    struct Node* node = root;
    while (node != NULL) {
        int comparison = strcmp(code, node->code);
        if (comparison == 0)
            return node->name;
        node = comparison < 0 ? node->left : node->right;
    }
    return NULL;
}

struct Node* createNode(char* code, char* name)
{
    struct Node* node = calloc(1, sizeof(struct Node));
    if (node == NULL)
        return NULL;

    node->code = strdup(code);
    node->name = strdup(name);
    if (node->code == NULL || node->name == NULL) {
        free(node->code);
        free(node->name);
        free(node);
        return NULL;
    }
    return node;
}

void freeNode(struct Node* node)
{
    free(node->code);
    free(node->name);
    free(node);
}

struct Node* insert(struct Node* node, struct Node* newNode, bool* isGrown)
{
    if (node == NULL) {
        *isGrown = true;
        return newNode;
    }

    if (strcmp(newNode->code, node->code) < 0) {
        node->left = insert(node->left, newNode, isGrown);
        if (*isGrown)
            node->balance--;
    } else {
        node->right = insert(node->right, newNode, isGrown);
        if (*isGrown)
            node->balance++;
    }

    if (!*isGrown)
        return node;

    node = balance(node);
    *isGrown = node->balance != 0;
    return node;
}

bool addAirport(struct Node** root, char* code, char* name)
{
    if (findAirport(*root, code) != NULL)
        return false;

    struct Node* newNode = createNode(code, name);
    if (newNode == NULL)
        return false;

    bool isGrown = false;
    *root = insert(*root, newNode, &isGrown);
    return true;
}

void swapData(struct Node* first, struct Node* second)
{
    char* code = first->code;
    char* name = first->name;
    first->code = second->code;
    first->name = second->name;
    second->code = code;
    second->name = name;
}

struct Node* removeNode(struct Node* node, char* code, bool* isShrunk)
{
    if (node == NULL) {
        *isShrunk = false;
        return NULL;
    }

    int comparison = strcmp(code, node->code);

    if (comparison == 0 && (node->left == NULL || node->right == NULL)) {
        struct Node* child = node->left != NULL ? node->left : node->right;
        freeNode(node);
        *isShrunk = true;
        return child;
    }

    if (comparison == 0) {
        struct Node* next = node->right;
        while (next->left != NULL)
            next = next->left;
        swapData(node, next);
        comparison = 1;
    }

    if (comparison < 0) {
        node->left = removeNode(node->left, code, isShrunk);
        if (*isShrunk)
            node->balance++;
    } else {
        node->right = removeNode(node->right, code, isShrunk);
        if (*isShrunk)
            node->balance--;
    }

    if (!*isShrunk)
        return node;

    node = balance(node);
    *isShrunk = node->balance == 0;
    return node;
}

bool deleteAirport(struct Node** root, char* code)
{
    if (findAirport(*root, code) == NULL)
        return false;

    bool isShrunk = false;
    *root = removeNode(*root, code, &isShrunk);
    return true;
}

int saveTree(FILE* file, struct Node* node)
{
    if (node == NULL)
        return 0;

    int count = saveTree(file, node->left);
    fprintf(file, "%s:%s\n", node->code, node->name);
    count++;
    return count + saveTree(file, node->right);
}

void freeTree(struct Node* node)
{
    if (node == NULL)
        return;

    freeTree(node->left);
    freeTree(node->right);
    freeNode(node);
}
