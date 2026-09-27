#pragma once

#include <stdbool.h>

struct QueueElement {
    int value;
    struct QueueElement* next;
};

struct Queue {
    struct QueueElement* head;
    struct QueueElement* tail;
};

// Создаёт пустую очередь, возвращает NULL, если не хватило памяти
struct Queue* createQueue(void);

// Добавляет значение в конец очереди, возвращает false, если не хватило памяти
bool enqueue(struct Queue* queue, int value);

// Достаёт значение из начала очереди в value, возвращает false, если очередь пуста
bool dequeue(struct Queue* queue, int* value);

// Проверяет, пуста ли очередь
bool isEmpty(struct Queue* queue);

// Освобождает очередь вместе со всеми элементами
void freeQueue(struct Queue* queue);
