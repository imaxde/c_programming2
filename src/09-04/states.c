#include "states.h"
#include "priorityQueue.h"

#include <stdlib.h>

struct Map* createMap(int cityCount)
{
    struct Map* map = calloc(1, sizeof(struct Map));
    if (map == NULL)
        return NULL;

    map->graph = createGraph(cityCount);
    if (map->graph == NULL) {
        free(map);
        return NULL;
    }

    map->owner = calloc(cityCount, sizeof(int));
    if (map->owner == NULL) {
        freeMap(map);
        return NULL;
    }
    for (int city = 0; city < cityCount; city++)
        map->owner[city] = -1;
    return map;
}

void freeMap(struct Map* map)
{
    if (map == NULL)
        return;

    freeGraph(map->graph);
    free(map->owner);
    free(map);
}

bool isCity(struct Map* map, int number)
{
    return number >= 1 && number <= map->graph->cityCount;
}

struct Map* readMap(FILE* file)
{
    int cityCount = 0;
    int roadCount = 0;
    if (fscanf(file, "%d%d", &cityCount, &roadCount) != 2 || roadCount < 0)
        return NULL;

    struct Map* map = createMap(cityCount);
    if (map == NULL)
        return NULL;

    for (int i = 0; i < roadCount; i++) {
        int first = 0;
        int second = 0;
        int length = 0;
        if (fscanf(file, "%d%d%d", &first, &second, &length) != 3
            || !isCity(map, first) || !isCity(map, second) || length < 0
            || !addRoad(map->graph, first - 1, second - 1, length)) {
            freeMap(map);
            return NULL;
        }
    }

    if (fscanf(file, "%d", &map->stateCount) != 1 || map->stateCount <= 0 || map->stateCount > cityCount) {
        freeMap(map);
        return NULL;
    }

    for (int state = 0; state < map->stateCount; state++) {
        int capital = 0;
        if (fscanf(file, "%d", &capital) != 1 || !isCity(map, capital) || map->owner[capital - 1] != -1) {
            freeMap(map);
            return NULL;
        }
        map->owner[capital - 1] = state;
    }
    return map;
}

// Кладёт в очередь государства дороги из города city в ничейные города
bool addRoadsFrom(struct Map* map, int city, struct PriorityQueue* queue)
{
    for (struct Road* road = map->graph->roads[city]; road != NULL; road = road->next) {
        if (map->owner[road->city] == -1 && !enqueue(queue, road->city, road->length))
            return false;
    }
    return true;
}

// Достаёт из очереди ближайший ничейный город, попавшиеся по пути занятые города выбрасываются
int takeNearestCity(struct Map* map, struct PriorityQueue* queue)
{
    int city = -1;
    while (dequeue(queue, &city)) {
        if (map->owner[city] == -1)
            return city;
    }
    return -1;
}

int distributeCities(struct Map* map)
{
    int stateCount = map->stateCount;
    int cityCount = map->graph->cityCount;
    struct PriorityQueue** queues = calloc(stateCount, sizeof(struct PriorityQueue*));
    // Государства, которые ещё могут расти, в порядке ходов
    int* activeStates = calloc(stateCount, sizeof(int));
    if (queues == NULL || activeStates == NULL) {
        free(queues);
        free(activeStates);
        return -1;
    }

    bool isEnoughMemory = true;
    for (int state = 0; state < stateCount && isEnoughMemory; state++) {
        queues[state] = createQueue();
        activeStates[state] = state;
        isEnoughMemory = queues[state] != NULL;
    }

    int ownedCount = 0;
    for (int city = 0; city < cityCount && isEnoughMemory; city++) {
        if (map->owner[city] != -1) {
            ownedCount++;
            isEnoughMemory = addRoadsFrom(map, city, queues[map->owner[city]]);
        }
    }

    // Если у государства кончились ничейные соседи, новых уже не появится,
    // поэтому оно выбывает из очереди ходов, а остальные сдвигаются, сохраняя порядок
    int activeCount = stateCount;
    while (isEnoughMemory && activeCount > 0 && ownedCount < cityCount) {
        int stillActiveCount = 0;
        for (int i = 0; i < activeCount && isEnoughMemory && ownedCount < cityCount; i++) {
            int state = activeStates[i];
            int city = takeNearestCity(map, queues[state]);
            if (city != -1) {
                map->owner[city] = state;
                ownedCount++;
                isEnoughMemory = addRoadsFrom(map, city, queues[state]);
                activeStates[stillActiveCount] = state;
                stillActiveCount++;
            }
        }
        activeCount = stillActiveCount;
    }

    for (int state = 0; state < stateCount; state++)
        freeQueue(queues[state]);
    free(queues);
    free(activeStates);
    return isEnoughMemory ? ownedCount : -1;
}

bool printStates(FILE* file, struct Map* map)
{
    int stateCount = map->stateCount;
    int cityCount = map->graph->cityCount;
    int* firstCity = calloc(stateCount, sizeof(int));
    int* nextCity = calloc(cityCount, sizeof(int));
    if (firstCity == NULL || nextCity == NULL) {
        free(firstCity);
        free(nextCity);
        return false;
    }

    // Списки городов каждого государства на массивах, города идут с конца,
    // поэтому после добавления в начало списка они стоят по возрастанию
    for (int state = 0; state < stateCount; state++)
        firstCity[state] = -1;
    for (int city = cityCount - 1; city >= 0; city--) {
        int state = map->owner[city];
        if (state != -1) {
            nextCity[city] = firstCity[state];
            firstCity[state] = city;
        }
    }

    for (int state = 0; state < stateCount; state++) {
        fprintf(file, "Государство %d:", state + 1);
        for (int city = firstCity[state]; city != -1; city = nextCity[city])
            fprintf(file, " %d", city + 1);
        fprintf(file, "\n");
    }

    free(firstCity);
    free(nextCity);
    return true;
}
