#include "automaton.h"

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

void addTransitions(struct Transition* transitions, int* count, int from, const char* symbols, int to)
{
    for (int i = 0; symbols[i] != '\0'; i++) {
        transitions[*count].from = from;
        transitions[*count].symbol = symbols[i];
        transitions[*count].to = to;
        (*count)++;
    }
}

struct Automaton* createNumberAutomaton(enum AutomatonStatus* status)
{
    char* digits = "0123456789";
    struct Transition transitions[100];
    int count = 0;
    addTransitions(transitions, &count, 0, "-", 1);
    addTransitions(transitions, &count, 0, digits, 2);
    addTransitions(transitions, &count, 0, ".", 3);
    addTransitions(transitions, &count, 1, digits, 2);
    addTransitions(transitions, &count, 1, ".", 3);
    addTransitions(transitions, &count, 2, digits, 2);
    addTransitions(transitions, &count, 2, ".", 3);
    addTransitions(transitions, &count, 2, "E", 5);
    addTransitions(transitions, &count, 3, digits, 4);
    addTransitions(transitions, &count, 4, digits, 4);
    addTransitions(transitions, &count, 4, "E", 5);
    addTransitions(transitions, &count, 5, "+-", 6);
    addTransitions(transitions, &count, 5, digits, 7);
    addTransitions(transitions, &count, 6, digits, 7);
    addTransitions(transitions, &count, 7, digits, 7);

    int acceptingStates[] = { 2, 4, 7 };
    return createAutomaton(count, transitions, 3, acceptingStates, 0, status);
}

bool hasCreateStatus(int transitionCount, struct Transition* transitions, int acceptingCount, int* acceptingStates,
    int startState, enum AutomatonStatus expected)
{
    enum AutomatonStatus status = AutomatonOk;
    struct Automaton* automaton = createAutomaton(transitionCount, transitions, acceptingCount, acceptingStates, startState, &status);
    bool result = status == expected && (automaton != NULL) == (expected == AutomatonOk);
    freeAutomaton(automaton);
    return result;
}

bool isCheckedAs(struct Automaton* automaton, char* string, bool expected, enum AutomatonStatus expectedStatus)
{
    enum AutomatonStatus status = AutomatonOk;
    bool result = isAccepted(automaton, string, &status);
    return result == expected && status == expectedStatus;
}

bool testCreateAutomaton(void)
{
    struct Transition good[] = { { 0, 'a', 1 }, { 0, 'a', 1 }, { 1, 'b', 0 } };
    struct Transition conflict[] = { { 0, 'a', 1 }, { 0, 'a', 2 } };
    struct Transition negative[] = { { 0, 'a', -1 } };
    int accepting[] = { 1 };
    int negativeAccepting[] = { -2 };

    return hasCreateStatus(3, good, 1, accepting, 0, AutomatonOk)
        && hasCreateStatus(0, NULL, 0, NULL, 0, AutomatonOk)
        && hasCreateStatus(2, conflict, 1, accepting, 0, AutomatonNotDeterministic)
        && hasCreateStatus(1, negative, 1, accepting, 0, AutomatonWrongDescription)
        && hasCreateStatus(3, good, 1, negativeAccepting, 0, AutomatonWrongDescription)
        && hasCreateStatus(3, good, 1, accepting, -1, AutomatonWrongDescription)
        && hasCreateStatus(-1, good, 1, accepting, 0, AutomatonWrongDescription)
        && hasCreateStatus(3, good, -1, accepting, 0, AutomatonWrongDescription)
        && hasCreateStatus(1, NULL, 1, accepting, 0, AutomatonNullArgument)
        && hasCreateStatus(3, good, 1, NULL, 0, AutomatonNullArgument);
}

bool testLectureAutomaton(void)
{
    struct Transition transitions[] = {
        { 0, 'a', 1 },
        { 0, 'b', 0 },
        { 1, 'a', 1 },
        { 1, 'b', 2 },
        { 2, 'a', 1 },
        { 2, 'b', 3 },
        { 3, 'a', 1 },
        { 3, 'b', 0 },
    };
    int accepting[] = { 3 };
    struct Automaton* automaton = createAutomaton(8, transitions, 1, accepting, 0, NULL);

    bool result = automaton != NULL
        && isCheckedAs(automaton, "abb", true, AutomatonOk)
        && isCheckedAs(automaton, "babaabb", true, AutomatonOk)
        && isCheckedAs(automaton, "abba", false, AutomatonOk)
        && isCheckedAs(automaton, "", false, AutomatonOk)
        && isCheckedAs(automaton, "abbc", false, AutomatonUnknownSymbol)
        && isAccepted(automaton, "aabb", NULL);
    freeAutomaton(automaton);
    return result;
}

bool testNoTransition(void)
{
    struct Transition transitions[] = { { 0, 'a', 1 }, { 1, 'b', 2 }, { 2, '\xd0', 3 }, { 3, '\xaf', 4 } };
    int accepting[] = { 2, 4 };
    struct Automaton* automaton = createAutomaton(4, transitions, 2, accepting, 0, NULL);

    bool result = automaton != NULL
        && isCheckedAs(automaton, "ab", true, AutomatonOk)
        && isCheckedAs(automaton, "abЯ", true, AutomatonOk)
        && isCheckedAs(automaton, "a", false, AutomatonOk)
        && isCheckedAs(automaton, "ba", false, AutomatonNoTransition)
        && isCheckedAs(automaton, "abb", false, AutomatonNoTransition)
        && isCheckedAs(automaton, "abЮ", false, AutomatonUnknownSymbol)
        && isCheckedAs(automaton, NULL, false, AutomatonNullArgument)
        && isCheckedAs(NULL, "ab", false, AutomatonNullArgument);
    freeAutomaton(automaton);
    return result;
}

bool testNumberAutomaton(void)
{
    enum AutomatonStatus status = AutomatonNoMemory;
    struct Automaton* automaton = createNumberAutomaton(&status);
    bool result = automaton != NULL && status == AutomatonOk
        && isCheckedAs(automaton, "38.871E5", true, AutomatonOk)
        && isCheckedAs(automaton, ".591", true, AutomatonOk)
        && isCheckedAs(automaton, "-12", true, AutomatonOk)
        && isCheckedAs(automaton, "-.5E-3", true, AutomatonOk)
        && isCheckedAs(automaton, "0", true, AutomatonOk)
        && isCheckedAs(automaton, "1E+10", true, AutomatonOk)
        && isCheckedAs(automaton, "А я число?", false, AutomatonUnknownSymbol)
        && isCheckedAs(automaton, "823.16.10", false, AutomatonNoTransition)
        && isCheckedAs(automaton, "", false, AutomatonOk)
        && isCheckedAs(automaton, "-", false, AutomatonOk)
        && isCheckedAs(automaton, "5.", false, AutomatonOk)
        && isCheckedAs(automaton, "1E-", false, AutomatonOk)
        && isCheckedAs(automaton, "--5", false, AutomatonNoTransition)
        && isCheckedAs(automaton, "+5", false, AutomatonNoTransition)
        && isCheckedAs(automaton, "E5", false, AutomatonNoTransition)
        && isCheckedAs(automaton, "1e5", false, AutomatonUnknownSymbol)
        && isCheckedAs(automaton, " 5", false, AutomatonUnknownSymbol);
    freeAutomaton(automaton);
    return result;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        if (!testCreateAutomaton() || !testLectureAutomaton() || !testNoTransition() || !testNumberAutomaton()) {
            printf("Тесты не пройдены\n");
            return 1;
        }
        printf("Все тесты пройдены\n");
        return 0;
    }

    enum AutomatonStatus status = AutomatonOk;
    struct Automaton* automaton = createNumberAutomaton(&status);
    if (automaton == NULL) {
        if (status == AutomatonNoMemory)
            printf("Не хватает памяти\n");
        else
            printf("Автомат для чисел задан неправильно\n");
        return 1;
    }

    printf("Введите строку: ");
    char* line = readLine(stdin);
    if (line == NULL) {
        printf("Не удалось прочитать строку\n");
        freeAutomaton(automaton);
        return 1;
    }

    if (isAccepted(automaton, line, NULL))
        printf("Это число!\n");
    else
        printf("Это не число :(\n");

    free(line);
    freeAutomaton(automaton);
    return 0;
}
