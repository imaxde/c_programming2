#pragma once

#include <stdbool.h>

// Переход из состояния from в состояние to по символу symbol
struct Transition {
    int from;
    char symbol;
    int to;
};

enum AutomatonStatus {
    // Всё прошло без ошибок
    AutomatonOk,
    // Не хватило памяти
    AutomatonNoMemory,
    // Вместо массива, автомата или строки передан NULL
    AutomatonNullArgument,
    // Отрицательное число переходов или допускающих состояний, отрицательный номер состояния
    AutomatonWrongDescription,
    // Из одного состояния по одному символу есть переходы в разные состояния
    AutomatonNotDeterministic,
    // В строке встретился символ не из алфавита автомата
    AutomatonUnknownSymbol,
    // Из текущего состояния нет перехода по очередному символу
    AutomatonNoTransition,
};

// ДКА, состояния нумеруются с 0, алфавит состоит из символов, которые встречаются в переходах
struct Automaton;

// Создаёт ДКА по transitionCount переходам, acceptingCount допускающим состояниям и стартовому состоянию
// В status записывает результат, при ошибке возвращает NULL
struct Automaton* createAutomaton(int transitionCount, struct Transition* transitions,
    int acceptingCount, int* acceptingStates, int startState, enum AutomatonStatus* status);

// Освобождает автомат
void freeAutomaton(struct Automaton* automaton);

// Проверяет, принадлежит ли строка языку автомата
// В status записывает AutomatonOk, если строка прочитана целиком, иначе причину, по которой чтение остановилось
// status может быть NULL, если причина не нужна
bool isAccepted(struct Automaton* automaton, const char* string, enum AutomatonStatus* status);
