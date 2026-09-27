#pragma once

#include <stdbool.h>

struct Road {
    int city;
    int length;
    struct Road* next;
};

struct Graph {
    int cityCount;
    struct Road** roads;
};

// Создаёт граф из cityCount городов без дорог, возвращает NULL, если городов нет или не хватило памяти
struct Graph* createGraph(int cityCount);

// Освобождает граф вместе со всеми дорогами
void freeGraph(struct Graph* graph);

// Добавляет дорогу длины length между городами first и second, возвращает false, если не хватило памяти
bool addRoad(struct Graph* graph, int first, int second, int length);
