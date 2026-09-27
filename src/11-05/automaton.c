#include "automaton.h"

#include <stdlib.h>

#define SYMBOL_COUNT 256

struct Automaton {
    int stateCount;
    int startState;
    int** next;
    bool* isAccepting;
    bool isInAlphabet[SYMBOL_COUNT];
};

void setStatus(enum AutomatonStatus* status, enum AutomatonStatus value)
{
    if (status != NULL)
        *status = value;
}

int max(int first, int second)
{
    if (first > second)
        return first;
    return second;
}

int countStates(int transitionCount, struct Transition* transitions, int acceptingCount, int* acceptingStates, int startState)
{
    bool isCorrect = startState >= 0;
    int maxState = startState;
    for (int i = 0; i < transitionCount; i++) {
        isCorrect = isCorrect && transitions[i].from >= 0 && transitions[i].to >= 0;
        maxState = max(maxState, max(transitions[i].from, transitions[i].to));
    }
    for (int i = 0; i < acceptingCount; i++) {
        isCorrect = isCorrect && acceptingStates[i] >= 0;
        maxState = max(maxState, acceptingStates[i]);
    }
    if (!isCorrect)
        return -1;
    return maxState + 1;
}

void freeAutomaton(struct Automaton* automaton)
{
    if (automaton == NULL)
        return;

    if (automaton->next != NULL) {
        for (int state = 0; state < automaton->stateCount; state++)
            free(automaton->next[state]);
    }
    free(automaton->next);
    free(automaton->isAccepting);
    free(automaton);
}

struct Automaton* allocateAutomaton(int stateCount)
{
    struct Automaton* automaton = calloc(1, sizeof(struct Automaton));
    if (automaton == NULL)
        return NULL;

    automaton->stateCount = stateCount;
    automaton->next = calloc(stateCount, sizeof(int*));
    automaton->isAccepting = calloc(stateCount, sizeof(bool));
    if (automaton->next == NULL || automaton->isAccepting == NULL) {
        freeAutomaton(automaton);
        return NULL;
    }

    for (int state = 0; state < stateCount; state++) {
        automaton->next[state] = malloc(SYMBOL_COUNT * sizeof(int));
        if (automaton->next[state] == NULL) {
            freeAutomaton(automaton);
            return NULL;
        }
        for (int symbol = 0; symbol < SYMBOL_COUNT; symbol++)
            automaton->next[state][symbol] = -1;
    }
    return automaton;
}

struct Automaton* createAutomaton(int transitionCount, struct Transition* transitions,
    int acceptingCount, int* acceptingStates, int startState, enum AutomatonStatus* status)
{
    if ((transitionCount > 0 && transitions == NULL) || (acceptingCount > 0 && acceptingStates == NULL)) {
        setStatus(status, AutomatonNullArgument);
        return NULL;
    }

    int stateCount = -1;
    if (transitionCount >= 0 && acceptingCount >= 0)
        stateCount = countStates(transitionCount, transitions, acceptingCount, acceptingStates, startState);
    if (stateCount < 0) {
        setStatus(status, AutomatonWrongDescription);
        return NULL;
    }

    struct Automaton* automaton = allocateAutomaton(stateCount);
    if (automaton == NULL) {
        setStatus(status, AutomatonNoMemory);
        return NULL;
    }
    automaton->startState = startState;

    for (int i = 0; i < transitionCount; i++) {
        unsigned char symbol = transitions[i].symbol;
        int* next = &automaton->next[transitions[i].from][symbol];
        if (*next != -1 && *next != transitions[i].to) {
            freeAutomaton(automaton);
            setStatus(status, AutomatonNotDeterministic);
            return NULL;
        }
        *next = transitions[i].to;
        automaton->isInAlphabet[symbol] = true;
    }

    for (int i = 0; i < acceptingCount; i++)
        automaton->isAccepting[acceptingStates[i]] = true;

    setStatus(status, AutomatonOk);
    return automaton;
}

bool isAccepted(struct Automaton* automaton, const char* string, enum AutomatonStatus* status)
{
    if (automaton == NULL || string == NULL) {
        setStatus(status, AutomatonNullArgument);
        return false;
    }

    int state = automaton->startState;
    for (int i = 0; string[i] != '\0'; i++) {
        unsigned char symbol = string[i];
        if (!automaton->isInAlphabet[symbol]) {
            setStatus(status, AutomatonUnknownSymbol);
            return false;
        }
        state = automaton->next[state][symbol];
        if (state == -1) {
            setStatus(status, AutomatonNoTransition);
            return false;
        }
    }

    setStatus(status, AutomatonOk);
    return automaton->isAccepting[state];
}
