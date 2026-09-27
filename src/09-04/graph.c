#include "graph.h"

#include <stdlib.h>

struct Graph* createGraph(int cityCount)
{
    if (cityCount <= 0)
        return NULL;

    struct Graph* graph = malloc(sizeof(struct Graph));
    if (graph == NULL)
        return NULL;

    graph->cityCount = cityCount;
    graph->roads = calloc(cityCount, sizeof(struct Road*));
    if (graph->roads == NULL) {
        free(graph);
        return NULL;
    }
    return graph;
}

void freeGraph(struct Graph* graph)
{
    if (graph == NULL)
        return;

    for (int city = 0; city < graph->cityCount; city++) {
        struct Road* road = graph->roads[city];
        while (road != NULL) {
            struct Road* next = road->next;
            free(road);
            road = next;
        }
    }
    free(graph->roads);
    free(graph);
}

bool addOneWayRoad(struct Graph* graph, int from, int to, int length)
{
    struct Road* road = malloc(sizeof(struct Road));
    if (road == NULL)
        return false;

    road->city = to;
    road->length = length;
    road->next = graph->roads[from];
    graph->roads[from] = road;
    return true;
}

bool addRoad(struct Graph* graph, int first, int second, int length)
{
    return addOneWayRoad(graph, first, second, length) && addOneWayRoad(graph, second, first, length);
}
