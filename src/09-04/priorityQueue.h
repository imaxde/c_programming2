#pragma once

#include <stdbool.h>

struct QueueItem {
    int value;
    int priority;
};

// Двоичная куча: у каждого элемента приоритет не больше, чем у его детей
struct PriorityQueue {
    int count;
    int capacity;
    struct QueueItem* items;
};

// Создаёт пустую очередь с приоритетами, возвращает NULL, если не хватило памяти
struct PriorityQueue* createQueue(void);

// Добавляет значение value с приоритетом priority, возвращает false, если не хватило памяти
bool enqueue(struct PriorityQueue* queue, int value, int priority);

// Достаёт в value значение с наименьшим приоритетом, из равных по приоритету наименьшее значение
// Возвращает false, если очередь пуста
bool dequeue(struct PriorityQueue* queue, int* value);

// Освобождает очередь
void freeQueue(struct PriorityQueue* queue);
