#include "avlTree.h"
#include "linkedList.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PEAK_FIND_COUNT 50000
#define UPDATE_INSERT_COUNT 10000

struct Dictionary {
    bool isTree;
    struct Node* tree;
    struct ListNode* list;
};

bool insertAirport(struct Dictionary* dictionary, char* code, char* name)
{
    if (dictionary->isTree)
        return addAirport(&dictionary->tree, code, name);
    return insertToList(&dictionary->list, code, name);
}

char* findName(struct Dictionary* dictionary, char* code)
{
    if (dictionary->isTree)
        return findAirport(dictionary->tree, code);
    return findInList(dictionary->list, code);
}

bool deleteCode(struct Dictionary* dictionary, char* code)
{
    if (dictionary->isTree)
        return deleteAirport(&dictionary->tree, code);
    return deleteFromList(&dictionary->list, code);
}

void freeDictionary(struct Dictionary* dictionary)
{
    freeTree(dictionary->tree);
    freeList(dictionary->list);
    dictionary->tree = NULL;
    dictionary->list = NULL;
}

struct Dataset {
    int count;
    int capacity;
    char** codes;
    char** names;
};

char* readLine(FILE* file)
{
    int c = fgetc(file);
    if (c == EOF)
        return NULL;

    int capacity = 64;
    char* line = malloc(capacity);
    if (line == NULL)
        return NULL;

    int length = 0;
    while (c != '\n' && c != EOF) {
        if (length + 1 == capacity) {
            capacity *= 2;
            char* newLine = realloc(line, capacity);
            if (newLine == NULL) {
                free(line);
                return NULL;
            }
            line = newLine;
        }
        line[length] = (char)c;
        length++;
        c = fgetc(file);
    }

    if (length > 0 && line[length - 1] == '\r')
        length--;
    line[length] = '\0';
    return line;
}

void freeDataset(struct Dataset* dataset)
{
    for (int i = 0; i < dataset->count; i++) {
        free(dataset->codes[i]);
        free(dataset->names[i]);
    }
    free(dataset->codes);
    free(dataset->names);
    dataset->count = 0;
    dataset->capacity = 0;
    dataset->codes = NULL;
    dataset->names = NULL;
}

bool addRecord(struct Dataset* dataset, char* code, char* name)
{
    if (dataset->count == dataset->capacity) {
        int newCapacity = dataset->capacity == 0 ? 1024 : dataset->capacity * 2;
        char** newCodes = realloc(dataset->codes, newCapacity * sizeof(char*));
        if (newCodes == NULL)
            return false;
        dataset->codes = newCodes;
        char** newNames = realloc(dataset->names, newCapacity * sizeof(char*));
        if (newNames == NULL)
            return false;
        dataset->names = newNames;
        dataset->capacity = newCapacity;
    }

    char* codeCopy = strdup(code);
    char* nameCopy = strdup(name);
    if (codeCopy == NULL || nameCopy == NULL) {
        free(codeCopy);
        free(nameCopy);
        return false;
    }
    dataset->codes[dataset->count] = codeCopy;
    dataset->names[dataset->count] = nameCopy;
    dataset->count++;
    return true;
}

bool readDataset(FILE* file, struct Dataset* dataset)
{
    dataset->count = 0;
    dataset->capacity = 0;
    dataset->codes = NULL;
    dataset->names = NULL;

    while (!feof(file) && !ferror(file)) {
        char* line = readLine(file);
        if (line == NULL)
            break;

        char* separator = strchr(line, ':');
        bool isAdded = true;
        if (separator != NULL && separator != line && separator[1] != '\0') {
            *separator = '\0';
            isAdded = addRecord(dataset, line, separator + 1);
        }
        free(line);
        if (!isAdded) {
            freeDataset(dataset);
            return false;
        }
    }

    if (!feof(file) || ferror(file)) {
        freeDataset(dataset);
        return false;
    }
    return true;
}

bool loadDictionary(struct Dictionary* dictionary, struct Dataset* dataset)
{
    for (int i = 0; i < dataset->count; i++) {
        if (!insertAirport(dictionary, dataset->codes[i], dataset->names[i])
            && findName(dictionary, dataset->codes[i]) == NULL)
            return false;
    }
    return true;
}

int randomState = 1;

int randomNumber(int limit)
{
    randomState = (randomState * 75 + 74) % 65537;
    return randomState % limit;
}

int runPeakHour(struct Dictionary* dictionary, struct Dataset* dataset)
{
    if (dataset->count == 0)
        return 0;

    randomState = 1;
    int foundCount = 0;
    for (int i = 0; i < PEAK_FIND_COUNT; i++) {
        char* code = dataset->codes[randomNumber(dataset->count)];
        if (findName(dictionary, code) != NULL)
            foundCount++;
    }
    return foundCount;
}

int runUpdate(struct Dictionary* dictionary, struct Dataset* dataset)
{
    if (dataset->count == 0)
        return 0;

    randomState = 1;
    int successCount = 0;
    char code[16];
    for (int i = 0; i < UPDATE_INSERT_COUNT; i++) {
        snprintf(code, sizeof(code), "N%04d", i);
        if (insertAirport(dictionary, code, "New Airport"))
            successCount++;

        char* existingCode = dataset->codes[randomNumber(dataset->count)];
        if (findName(dictionary, existingCode) != NULL)
            successCount++;
    }
    return successCount;
}

bool testList(void)
{
    struct ListNode* head = NULL;
    bool result = insertToList(&head, "SVO", "Sheremetyevo International Airport")
        && insertToList(&head, "LED", "Pulkovo Airport")
        && !insertToList(&head, "SVO", "Other Airport")
        && strcmp(findInList(head, "SVO"), "Sheremetyevo International Airport") == 0
        && strcmp(findInList(head, "LED"), "Pulkovo Airport") == 0
        && findInList(head, "ZZZ") == NULL
        && deleteFromList(&head, "SVO")
        && !deleteFromList(&head, "SVO")
        && findInList(head, "SVO") == NULL
        && deleteFromList(&head, "LED")
        && head == NULL;
    freeList(head);
    return result;
}

bool testSameInterface(bool isTree)
{
    struct Dictionary dictionary = { isTree, NULL, NULL };
    bool result = insertAirport(&dictionary, "JFK", "John F Kennedy International Airport")
        && insertAirport(&dictionary, "AMS", "Amsterdam Airport Schiphol")
        && !insertAirport(&dictionary, "JFK", "Duplicate")
        && strcmp(findName(&dictionary, "JFK"), "John F Kennedy International Airport") == 0
        && findName(&dictionary, "LED") == NULL
        && deleteCode(&dictionary, "JFK")
        && !deleteCode(&dictionary, "JFK")
        && findName(&dictionary, "JFK") == NULL
        && findName(&dictionary, "AMS") != NULL;
    freeDictionary(&dictionary);
    return result;
}

bool testRandomNumber(void)
{
    randomState = 1;
    for (int i = 0; i < 100000; i++) {
        int number = randomNumber(9054);
        if (number < 0 || number >= 9054)
            return false;
    }
    randomState = 1;
    int first = randomNumber(1000);
    randomState = 1;
    return randomNumber(1000) == first;
}

bool testReadDataset(void)
{
    FILE* file = tmpfile();
    if (file == NULL)
        return false;

    struct Dataset dataset;
    bool result = fputs("SVO:Sheremetyevo\r\nbroken\n:NoCode\nLED:Pulkovo", file) != EOF
        && fseek(file, 0, SEEK_SET) == 0
        && readDataset(file, &dataset);
    fclose(file);
    if (!result)
        return false;

    result = dataset.count == 2
        && strcmp(dataset.codes[0], "SVO") == 0 && strcmp(dataset.names[0], "Sheremetyevo") == 0
        && strcmp(dataset.codes[1], "LED") == 0 && strcmp(dataset.names[1], "Pulkovo") == 0;
    freeDataset(&dataset);
    return result;
}

bool testScenariosGiveSameResults(void)
{
    struct Dataset dataset = { 0, 0, NULL, NULL };
    char code[16];
    bool result = true;
    for (int i = 0; i < 300 && result; i++) {
        snprintf(code, sizeof(code), "A%03d", i);
        result = addRecord(&dataset, code, "Airport");
    }

    struct Dictionary tree = { true, NULL, NULL };
    struct Dictionary list = { false, NULL, NULL };
    result = result && loadDictionary(&tree, &dataset) && loadDictionary(&list, &dataset)
        && runPeakHour(&tree, &dataset) == PEAK_FIND_COUNT
        && runPeakHour(&list, &dataset) == PEAK_FIND_COUNT
        && runUpdate(&tree, &dataset) == 2 * UPDATE_INSERT_COUNT
        && runUpdate(&list, &dataset) == 2 * UPDATE_INSERT_COUNT
        && findName(&tree, "N9999") != NULL && findName(&list, "N9999") != NULL;

    freeDictionary(&tree);
    freeDictionary(&list);
    freeDataset(&dataset);
    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        if (!testList() || !testSameInterface(true) || !testSameInterface(false)
            || !testRandomNumber() || !testReadDataset() || !testScenariosGiveSameResults()) {
            printf("Тесты не пройдены.\n");
            return 1;
        }
        printf("Все тесты пройдены.\n");
        return 0;
    }

    if (argc < 3 || argc > 4) {
        printf("Использование: %s <tree|list> <load|peak|update> [airports.txt]\n", argv[0]);
        return 1;
    }

    char* structure = argv[1];
    char* scenario = argv[2];
    char* path = argc == 4 ? argv[3] : "airports.txt";
    bool isTree = strcmp(structure, "tree") == 0;
    if (!isTree && strcmp(structure, "list") != 0) {
        printf("Неизвестная структура: %s\n", structure);
        return 1;
    }
    if (strcmp(scenario, "load") != 0 && strcmp(scenario, "peak") != 0 && strcmp(scenario, "update") != 0) {
        printf("Неизвестный сценарий: %s\n", scenario);
        return 1;
    }

    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror(path);
        return 1;
    }
    struct Dataset dataset;
    bool isRead = readDataset(file, &dataset);
    fclose(file);
    if (!isRead) {
        printf("Не удалось прочитать %s\n", path);
        return 1;
    }

    struct Dictionary dictionary = { isTree, NULL, NULL };
    if (!loadDictionary(&dictionary, &dataset)) {
        printf("Не хватает памяти\n");
        freeDictionary(&dictionary);
        freeDataset(&dataset);
        return 1;
    }

    int result = 0;
    if (strcmp(scenario, "peak") == 0)
        result = runPeakHour(&dictionary, &dataset);
    else if (strcmp(scenario, "update") == 0)
        result = runUpdate(&dictionary, &dataset);
    printf("%s %s: %d\n", structure, scenario, result);

    freeDictionary(&dictionary);
    freeDataset(&dataset);
    return 0;
}
