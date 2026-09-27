#include "queue.h"

#include <stdlib.h>

struct Queue* createQueue(void)
{
    return calloc(1, sizeof(struct Queue));
}

bool enqueue(struct Queue* queue, int value)
{
    struct QueueElement* element = malloc(sizeof(struct QueueElement));
    if (element == NULL)
        return false;

    element->value = value;
    element->next = NULL;
    if (queue->tail == NULL)
        queue->head = element;
    else
        queue->tail->next = element;
    queue->tail = element;
    return true;
}

bool dequeue(struct Queue* queue, int* value)
{
    if (queue->head == NULL)
        return false;

    struct QueueElement* element = queue->head;
    *value = element->value;
    queue->head = element->next;
    if (queue->head == NULL)
        queue->tail = NULL;
    free(element);
    return true;
}

bool isEmpty(struct Queue* queue)
{
    return queue->head == NULL;
}

void freeQueue(struct Queue* queue)
{
    if (queue == NULL)
        return;

    int value = 0;
    while (dequeue(queue, &value)) { }
    free(queue);
}
