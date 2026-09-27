#include "graph.h"
#include "queue.h"

#include <stdlib.h>

struct Graph* createGraph(int vertexCount)
{
    if (vertexCount <= 0)
        return NULL;

    struct Graph* graph = malloc(sizeof(struct Graph));
    if (graph == NULL)
        return NULL;

    graph->vertexCount = vertexCount;
    graph->edges = calloc(vertexCount, sizeof(bool*));
    if (graph->edges == NULL) {
        free(graph);
        return NULL;
    }

    for (int i = 0; i < vertexCount; i++) {
        graph->edges[i] = calloc(vertexCount, sizeof(bool));
        if (graph->edges[i] == NULL) {
            freeGraph(graph);
            return NULL;
        }
    }
    return graph;
}

void freeGraph(struct Graph* graph)
{
    if (graph == NULL)
        return;

    for (int i = 0; i < graph->vertexCount; i++)
        free(graph->edges[i]);
    free(graph->edges);
    free(graph);
}

void addEdge(struct Graph* graph, int from, int to)
{
    graph->edges[from][to] = true;
}

bool hasEdge(struct Graph* graph, int from, int to)
{
    return graph->edges[from][to];
}

struct Graph* readGraph(FILE* file)
{
    int vertexCount = 0;
    if (fscanf(file, "%d", &vertexCount) != 1)
        return NULL;

    struct Graph* graph = createGraph(vertexCount);
    if (graph == NULL)
        return NULL;

    for (int from = 0; from < vertexCount; from++) {
        for (int to = 0; to < vertexCount; to++) {
            int value = 0;
            if (fscanf(file, "%d", &value) != 1) {
                freeGraph(graph);
                return NULL;
            }
            if (value != 0)
                addEdge(graph, from, to);
        }
    }
    return graph;
}

int breadthFirstSearch(struct Graph* graph, int start, int* order)
{
    bool* visited = calloc(graph->vertexCount, sizeof(bool));
    struct Queue* queue = createQueue();
    if (visited == NULL || queue == NULL) {
        free(visited);
        freeQueue(queue);
        return -1;
    }

    int count = 0;
    visited[start] = true;
    bool isEnqueued = enqueue(queue, start);
    int vertex = 0;
    while (isEnqueued && dequeue(queue, &vertex)) {
        order[count] = vertex;
        count++;
        for (int next = 0; next < graph->vertexCount && isEnqueued; next++) {
            if (hasEdge(graph, vertex, next) && !visited[next]) {
                visited[next] = true;
                isEnqueued = enqueue(queue, next);
            }
        }
    }

    free(visited);
    freeQueue(queue);
    return isEnqueued ? count : -1;
}
