#include "avlTree.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int loadAirports(FILE* file, struct Node** root)
{
    int count = 0;
    while (!feof(file) && !ferror(file)) {
        char* line = readLine(file);
        if (line == NULL)
            break;

        char* separator = strchr(line, ':');
        if (separator != NULL && separator != line && separator[1] != '\0') {
            *separator = '\0';
            if (findAirport(*root, line) == NULL) {
                if (!addAirport(root, line, separator + 1)) {
                    free(line);
                    return -1;
                }
                count++;
            }
        }
        free(line);
    }
    return feof(file) && !ferror(file) ? count : -1;
}

char* airportsWord(int count)
{
    if (count % 100 >= 11 && count % 100 <= 14)
        return "аэропортов";
    if (count % 10 == 1)
        return "аэропорт";
    if (count % 10 >= 2 && count % 10 <= 4)
        return "аэропорта";
    return "аэропортов";
}

char* skipSpaces(char* text)
{
    while (*text == ' ' || *text == '\t')
        text++;
    return text;
}

void trimEnd(char* text)
{
    int end = 0;
    for (int i = 0; text[i] != '\0'; i++) {
        if (text[i] != ' ' && text[i] != '\t')
            end = i + 1;
    }
    text[end] = '\0';
}

void findCommand(struct Node* root, char* code)
{
    if (*code == '\0') {
        printf("Укажите код аэропорта: find <код>\n");
        return;
    }

    char* name = findAirport(root, code);
    if (name == NULL)
        printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
    else
        printf("%s → %s\n", code, name);
}

void addCommand(struct Node** root, char* argument)
{
    char* separator = strchr(argument, ':');
    if (separator == NULL) {
        printf("Укажите код и название аэропорта: add <код>:<название>\n");
        return;
    }

    *separator = '\0';
    char* code = argument;
    trimEnd(code);
    char* name = skipSpaces(separator + 1);
    if (*code == '\0' || *name == '\0') {
        printf("Укажите код и название аэропорта: add <код>:<название>\n");
        return;
    }

    if (findAirport(*root, code) != NULL)
        printf("Аэропорт с кодом '%s' уже есть в базе.\n", code);
    else if (!addAirport(root, code, name))
        printf("Не хватает памяти, аэропорт '%s' не добавлен.\n", code);
    else
        printf("Аэропорт '%s' добавлен в базу.\n", code);
}

void deleteCommand(struct Node** root, char* code)
{
    if (*code == '\0') {
        printf("Укажите код аэропорта: delete <код>\n");
        return;
    }

    if (deleteAirport(root, code))
        printf("Аэропорт '%s' удалён из базы.\n", code);
    else
        printf("Аэропорт с кодом '%s' не найден в базе.\n", code);
}

void saveCommand(struct Node* root, char* path)
{
    FILE* file = fopen(path, "w");
    if (file == NULL) {
        perror(path);
        return;
    }

    int count = saveTree(file, root);
    bool isWritten = !ferror(file);
    bool isClosed = fclose(file) == 0;
    if (isWritten && isClosed)
        printf("База сохранена: %d %s.\n", count, airportsWord(count));
    else
        printf("Не удалось сохранить базу в \"%s\".\n", path);
}

bool runCommand(struct Node** root, char* line, char* path)
{
    char* command = skipSpaces(line);
    char* argument = command;
    while (*argument != '\0' && *argument != ' ' && *argument != '\t')
        argument++;
    if (*argument != '\0') {
        *argument = '\0';
        argument = skipSpaces(argument + 1);
    }
    trimEnd(argument);

    if (strcmp(command, "quit") == 0)
        return false;

    if (strcmp(command, "find") == 0)
        findCommand(*root, argument);
    else if (strcmp(command, "add") == 0)
        addCommand(root, argument);
    else if (strcmp(command, "delete") == 0)
        deleteCommand(root, argument);
    else if (strcmp(command, "save") == 0)
        saveCommand(*root, path);
    else if (*command != '\0')
        printf("Неизвестная команда '%s'. Доступные команды: find, add, delete, save, quit.\n", command);
    return true;
}

int checkNode(struct Node* node, char** previousCode, bool* isCorrect)
{
    if (node == NULL)
        return 0;

    int leftHeight = checkNode(node->left, previousCode, isCorrect);
    if (*previousCode != NULL && strcmp(*previousCode, node->code) >= 0)
        *isCorrect = false;
    *previousCode = node->code;
    int rightHeight = checkNode(node->right, previousCode, isCorrect);

    if (node->balance != rightHeight - leftHeight || node->balance < -1 || node->balance > 1)
        *isCorrect = false;
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

bool isCorrectAvlTree(struct Node* root)
{
    char* previousCode = NULL;
    bool isCorrect = true;
    checkNode(root, &previousCode, &isCorrect);
    return isCorrect;
}

bool testFindAddDelete(void)
{
    struct Node* root = NULL;
    bool result = addAirport(&root, "SVO", "Sheremetyevo International Airport")
        && addAirport(&root, "LED", "Pulkovo Airport")
        && !addAirport(&root, "SVO", "Other Airport")
        && strcmp(findAirport(root, "SVO"), "Sheremetyevo International Airport") == 0
        && strcmp(findAirport(root, "LED"), "Pulkovo Airport") == 0
        && findAirport(root, "ZZZ") == NULL
        && deleteAirport(&root, "SVO")
        && !deleteAirport(&root, "SVO")
        && findAirport(root, "SVO") == NULL
        && findAirport(root, "LED") != NULL
        && deleteAirport(&root, "LED")
        && root == NULL;
    freeTree(root);
    return result;
}

bool testBalanceAfterAdding(void)
{
    struct Node* root = NULL;
    bool result = true;
    char code[8];
    for (int i = 0; i < 1000 && result; i++) {
        snprintf(code, sizeof(code), "%04d", i);
        result = addAirport(&root, code, "Airport") && isCorrectAvlTree(root);
    }
    for (int i = 1999; i >= 1000 && result; i--) {
        snprintf(code, sizeof(code), "%04d", i);
        result = addAirport(&root, code, "Airport") && isCorrectAvlTree(root);
    }
    freeTree(root);
    return result;
}

bool testDoubleRotations(void)
{
    struct Node* leftRight = NULL;
    struct Node* rightLeft = NULL;
    bool result = addAirport(&leftRight, "CCC", "C") && addAirport(&leftRight, "AAA", "A")
        && addAirport(&leftRight, "BBB", "B") && strcmp(leftRight->code, "BBB") == 0
        && isCorrectAvlTree(leftRight)
        && addAirport(&rightLeft, "AAA", "A") && addAirport(&rightLeft, "CCC", "C")
        && addAirport(&rightLeft, "BBB", "B") && strcmp(rightLeft->code, "BBB") == 0
        && isCorrectAvlTree(rightLeft);
    freeTree(leftRight);
    freeTree(rightLeft);
    return result;
}

bool testBalanceAfterDeleting(void)
{
    struct Node* root = NULL;
    bool result = true;
    char code[8];
    for (int i = 0; i < 1000 && result; i++) {
        snprintf(code, sizeof(code), "%04d", i * 7 % 1000);
        result = addAirport(&root, code, "Airport") && isCorrectAvlTree(root);
    }
    for (int i = 0; i < 1000 && result; i += 3) {
        snprintf(code, sizeof(code), "%04d", i);
        result = deleteAirport(&root, code) && isCorrectAvlTree(root);
    }
    for (int i = 0; i < 1000 && result; i++) {
        snprintf(code, sizeof(code), "%04d", i);
        bool isDeleted = i % 3 == 0;
        result = (findAirport(root, code) == NULL) == isDeleted;
    }
    for (int i = 999; i >= 0 && result; i--) {
        snprintf(code, sizeof(code), "%04d", i);
        bool isDeleted = i % 3 == 0;
        result = deleteAirport(&root, code) != isDeleted && isCorrectAvlTree(root);
    }
    result = result && root == NULL;
    freeTree(root);
    return result;
}

bool hasContent(FILE* file, char* expected)
{
    if (fseek(file, 0, SEEK_SET) != 0)
        return false;

    for (char* current = expected; *current != '\0'; current++) {
        if (fgetc(file) != (unsigned char)*current)
            return false;
    }
    return fgetc(file) == EOF;
}

bool testLoadAndSave(void)
{
    char* data = "SVO:Sheremetyevo International Airport\r\n"
                 "\n"
                 "broken line\n"
                 ":No Code\n"
                 "LED:Pulkovo Airport\n"
                 "SVO:Duplicate\n"
                 "AMS:Amsterdam Airport Schiphol";
    FILE* input = tmpfile();
    FILE* output = tmpfile();
    struct Node* root = NULL;
    bool result = input != NULL && output != NULL
        && fputs(data, input) != EOF
        && fseek(input, 0, SEEK_SET) == 0
        && loadAirports(input, &root) == 3
        && saveTree(output, root) == 3
        && hasContent(output,
            "AMS:Amsterdam Airport Schiphol\n"
            "LED:Pulkovo Airport\n"
            "SVO:Sheremetyevo International Airport\n");

    if (input != NULL)
        fclose(input);
    if (output != NULL)
        fclose(output);
    freeTree(root);
    return result;
}

bool testAirportsWord(void)
{
    return strcmp(airportsWord(1), "аэропорт") == 0
        && strcmp(airportsWord(3), "аэропорта") == 0
        && strcmp(airportsWord(5), "аэропортов") == 0
        && strcmp(airportsWord(11), "аэропортов") == 0
        && strcmp(airportsWord(21), "аэропорт") == 0
        && strcmp(airportsWord(9054), "аэропорта") == 0
        && strcmp(airportsWord(9096), "аэропортов") == 0;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        if (!testFindAddDelete() || !testDoubleRotations() || !testBalanceAfterAdding()
            || !testBalanceAfterDeleting() || !testLoadAndSave() || !testAirportsWord()) {
            printf("Тесты не пройдены.\n");
            return 1;
        }
        printf("Все тесты пройдены.\n");
        return 0;
    }

    if (argc > 2) {
        fprintf(stderr, "Использование: %s [airports.txt]\n", argv[0]);
        fprintf(stderr, "Запуск тестов: %s --test\n", argv[0]);
        return 1;
    }

    char* path = argc == 2 ? argv[1] : "airports.txt";
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror(path);
        return 1;
    }

    struct Node* root = NULL;
    int count = loadAirports(file, &root);
    fclose(file);
    if (count < 0) {
        fprintf(stderr, "Не удалось загрузить базу из \"%s\".\n", path);
        freeTree(root);
        return 1;
    }
    printf("Загружено %d %s. Система готова к работе.\n", count, airportsWord(count));

    bool isRunning = true;
    while (isRunning) {
        printf("\n> ");
        fflush(stdout);
        char* line = readLine(stdin);
        if (line == NULL) {
            printf("\n");
            break;
        }
        isRunning = runCommand(&root, line, path);
        free(line);
    }

    freeTree(root);
    return 0;
}
