#include "prettyCsv.h"

#include <stdlib.h>

bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool isSign(char c)
{
    return c == '+' || c == '-';
}

int countDigits(char* text)
{
    int count = 0;
    while (isDigit(text[count]))
        count++;
    return count;
}

bool isNumber(char* text)
{
    int position = 0;
    if (isSign(text[position]))
        position++;

    int integerDigits = countDigits(text + position);
    position += integerDigits;

    int fractionDigits = 0;
    if (text[position] == '.') {
        position++;
        fractionDigits = countDigits(text + position);
        position += fractionDigits;
    }

    if (integerDigits + fractionDigits == 0)
        return false;

    if (text[position] == 'e' || text[position] == 'E') {
        position++;
        if (isSign(text[position]))
            position++;

        int exponentDigits = countDigits(text + position);
        if (exponentDigits == 0)
            return false;
        position += exponentDigits;
    }

    return text[position] == '\0';
}

int readChar(FILE* csv)
{
    int c = fgetc(csv);
    if (c != '\r')
        return c;

    int next = fgetc(csv);
    if (next != '\n' && next != EOF)
        ungetc(next, csv);
    return '\n';
}

bool readCellCSV(FILE* csv, struct Cell* cell, int* terminator)
{
    int capacity = 16;
    char* text = malloc(capacity);
    if (text == NULL)
        return false;

    int length = 0;
    int c = readChar(csv);
    while (c != ',' && c != '\n' && c != EOF) {
        if (length + 1 == capacity) {
            capacity *= 2;
            char* newText = realloc(text, capacity);
            if (newText == NULL) {
                free(text);
                return false;
            }
            text = newText;
        }
        text[length] = (char)c;
        length++;
        c = readChar(csv);
    }
    text[length] = '\0';

    cell->length = length;
    cell->text = text;
    *terminator = c;
    return true;
}

void freeRow(struct Row* row)
{
    for (int i = 0; i < row->cellCount; i++)
        free(row->cells[i].text);
    free(row->cells);
    row->cellCount = 0;
    row->cells = NULL;
}

bool readRowCSV(FILE* csv, struct Row* row, int* terminator)
{
    int capacity = 4;
    row->cellCount = 0;
    row->cells = malloc(capacity * sizeof(struct Cell));
    if (row->cells == NULL)
        return false;

    do {
        struct Cell cell;
        if (!readCellCSV(csv, &cell, terminator)) {
            freeRow(row);
            return false;
        }

        if (row->cellCount == capacity) {
            capacity *= 2;
            struct Cell* newCells = realloc(row->cells, capacity * sizeof(struct Cell));
            if (newCells == NULL) {
                free(cell.text);
                freeRow(row);
                return false;
            }
            row->cells = newCells;
        }
        row->cells[row->cellCount] = cell;
        row->cellCount++;
    } while (*terminator == ',');

    return true;
}

bool isBlankRow(struct Row* row)
{
    return row->cellCount == 1 && row->cells[0].length == 0;
}

bool fitColumns(struct Table* table, struct Row* row)
{
    if (row->cellCount > table->columnCount) {
        int* newWidths = realloc(table->maxWidthNumbers, row->cellCount * sizeof(int));
        if (newWidths == NULL)
            return false;

        for (int i = table->columnCount; i < row->cellCount; i++)
            newWidths[i] = 0;
        table->maxWidthNumbers = newWidths;
        table->columnCount = row->cellCount;
    }

    for (int i = 0; i < row->cellCount; i++) {
        if (row->cells[i].length > table->maxWidthNumbers[i])
            table->maxWidthNumbers[i] = row->cells[i].length;
    }
    return true;
}

bool addRow(struct Table* table, int* capacity, struct Row* row)
{
    if (!fitColumns(table, row))
        return false;

    if (table->header.cellCount == 0) {
        table->header = *row;
        return true;
    }

    if (table->rowCount == *capacity) {
        *capacity *= 2;
        struct Row* newRows = realloc(table->rows, *capacity * sizeof(struct Row));
        if (newRows == NULL)
            return false;
        table->rows = newRows;
    }
    table->rows[table->rowCount] = *row;
    table->rowCount++;
    return true;
}

bool readCSV(FILE* csv, struct Table* table)
{
    *table = (struct Table) { 0 };
    int capacity = 16;
    table->rows = malloc(capacity * sizeof(struct Row));
    if (table->rows == NULL)
        return false;

    int terminator = '\n';
    while (terminator != EOF) {
        struct Row row;
        if (!readRowCSV(csv, &row, &terminator)) {
            freeTable(table);
            return false;
        }

        if (isBlankRow(&row)) {
            freeRow(&row);
        } else if (!addRow(table, &capacity, &row)) {
            freeRow(&row);
            freeTable(table);
            return false;
        }
    }

    if (ferror(csv)) {
        freeTable(table);
        return false;
    }
    return true;
}

void writeBorder(FILE* output, struct Table* table, char fill)
{
    for (int i = 0; i < table->columnCount; i++) {
        fputc('+', output);
        for (int j = 0; j < table->maxWidthNumbers[i] + 2; j++)
            fputc(fill, output);
    }
    fputs("+\n", output);
}

void writeRow(FILE* output, struct Table* table, struct Row* row, bool isHeader)
{
    for (int i = 0; i < table->columnCount; i++) {
        int width = table->maxWidthNumbers[i];
        if (i >= row->cellCount)
            fprintf(output, "| %*s ", width, "");
        else if (!isHeader && isNumber(row->cells[i].text))
            fprintf(output, "| %*s ", width, row->cells[i].text);
        else
            fprintf(output, "| %-*s ", width, row->cells[i].text);
    }
    fputs("|\n", output);
}

bool writeTable(FILE* output, struct Table* table)
{
    if (table->columnCount == 0)
        return true;

    writeBorder(output, table, '=');
    writeRow(output, table, &table->header, true);
    writeBorder(output, table, '=');

    for (int i = 0; i < table->rowCount; i++) {
        writeRow(output, table, &table->rows[i], false);
        writeBorder(output, table, '-');
    }

    return !ferror(output);
}

void freeTable(struct Table* table)
{
    freeRow(&table->header);
    for (int i = 0; i < table->rowCount; i++)
        freeRow(&table->rows[i]);
    free(table->rows);
    free(table->maxWidthNumbers);
    *table = (struct Table) { 0 };
}
