#pragma once

#include <stdbool.h>
#include <stdio.h>

struct Cell {
    int length;
    char* text;
};

struct Row {
    int cellCount;
    struct Cell* cells;
};

struct Table {
    struct Row header;
    int* maxWidthNumbers;
    int rowCount;
    int columnCount;
    struct Row* rows;
};

bool isNumber(char* text);

bool readCSV(FILE* csv, struct Table* table);
bool writeTable(FILE* output, struct Table* table);
void freeTable(struct Table* table);
