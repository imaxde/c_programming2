#pragma once

#include <stdbool.h>
#include <stdio.h>

struct Graph {
    int vertexCount;
    bool** edges;
};

// Создаёт граф из vertexCount вершин без рёбер, возвращает NULL, если вершин нет или не хватило памяти
struct Graph* createGraph(int vertexCount);

// Освобождает память графа
void freeGraph(struct Graph* graph);

// Добавляет ребро из вершины from в вершину to
void addEdge(struct Graph* graph, int from, int to);

// Проверяет, есть ли ребро из вершины from в вершину to
bool hasEdge(struct Graph* graph, int from, int to);

// Читает из файла число вершин и матрицу смежности, ненулевое число в матрице означает ребро
// Возвращает NULL, если файл некорректный или не хватило памяти
struct Graph* readGraph(FILE* file);

// Обходит граф в ширину из вершины start и записывает вершины в порядке обхода в order,
// order должен вмещать vertexCount чисел
// Возвращает число посещённых вершин или -1, если не хватило памяти
int breadthFirstSearch(struct Graph* graph, int start, int* order);
