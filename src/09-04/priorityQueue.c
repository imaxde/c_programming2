#include "priorityQueue.h"

#include <stdlib.h>

struct PriorityQueue* createQueue(void)
{
    struct PriorityQueue* queue = malloc(sizeof(struct PriorityQueue));
    if (queue == NULL)
        return NULL;

    queue->count = 0;
    queue->capacity = 4;
    queue->items = calloc(queue->capacity, sizeof(struct QueueItem));
    if (queue->items == NULL) {
        free(queue);
        return NULL;
    }
    return queue;
}

bool isLess(struct QueueItem first, struct QueueItem second)
{
    return first.priority < second.priority
        || (first.priority == second.priority && first.value < second.value);
}

void swapItems(struct QueueItem* items, int first, int second)
{
    struct QueueItem item = items[first];
    items[first] = items[second];
    items[second] = item;
}

bool enqueue(struct PriorityQueue* queue, int value, int priority)
{
    if (queue->count == queue->capacity) {
        struct QueueItem* items = realloc(queue->items, queue->capacity * 2 * sizeof(struct QueueItem));
        if (items == NULL)
            return false;
        queue->items = items;
        queue->capacity *= 2;
    }

    int index = queue->count;
    queue->items[index].value = value;
    queue->items[index].priority = priority;
    queue->count++;

    int parent = (index - 1) / 2;
    while (index > 0 && isLess(queue->items[index], queue->items[parent])) {
        swapItems(queue->items, index, parent);
        index = parent;
        parent = (index - 1) / 2;
    }
    return true;
}

bool dequeue(struct PriorityQueue* queue, int* value)
{
    if (queue->count == 0)
        return false;

    *value = queue->items[0].value;
    queue->count--;
    queue->items[0] = queue->items[queue->count];

    int index = 0;
    while (true) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = left + 1;
        if (left < queue->count && isLess(queue->items[left], queue->items[smallest]))
            smallest = left;
        if (right < queue->count && isLess(queue->items[right], queue->items[smallest]))
            smallest = right;
        if (smallest == index)
            return true;

        swapItems(queue->items, index, smallest);
        index = smallest;
    }
}

void freeQueue(struct PriorityQueue* queue)
{
    if (queue == NULL)
        return;

    free(queue->items);
    free(queue);
}
