#pragma once

#include <stdbool.h>
#include <stdio.h>

struct Node {
    char* code;
    char* name;
    int balance;
    struct Node* left;
    struct Node* right;
};

char* findAirport(struct Node* root, char* code);
bool addAirport(struct Node** root, char* code, char* name);
bool deleteAirport(struct Node** root, char* code);
int saveTree(FILE* file, struct Node* node);
void freeTree(struct Node* node);
