#include "graph.h"
#include "priorityQueue.h"
#include "states.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

struct Map* mapFromText(char* text)
{
    FILE* file = tmpfile();
    if (file == NULL)
        return NULL;

    struct Map* map = NULL;
    if (fputs(text, file) != EOF && fseek(file, 0, SEEK_SET) == 0)
        map = readMap(file);
    fclose(file);
    return map;
}

bool isWrongMap(char* text)
{
    struct Map* map = mapFromText(text);
    bool result = map == NULL;
    freeMap(map);
    return result;
}

bool hasRoad(struct Graph* graph, int from, int to, int length)
{
    for (struct Road* road = graph->roads[from]; road != NULL; road = road->next) {
        if (road->city == to && road->length == length)
            return true;
    }
    return false;
}

bool isPrinted(struct Map* map, char* expected)
{
    FILE* file = tmpfile();
    if (file == NULL)
        return false;

    char text[256] = "";
    bool result = printStates(file, map) && fseek(file, 0, SEEK_SET) == 0
        && fread(text, 1, sizeof(text) - 1, file) > 0 && strcmp(text, expected) == 0;
    fclose(file);
    return result;
}

bool isDistribution(char* text, char* expected)
{
    struct Map* map = mapFromText(text);
    bool result = map != NULL && distributeCities(map) == map->graph->cityCount && isPrinted(map, expected);
    freeMap(map);
    return result;
}

bool testPriorityQueue(void)
{
    struct PriorityQueue* queue = createQueue();
    if (queue == NULL)
        return false;

    int value = 0;
    bool result = !dequeue(queue, &value);
    for (int i = 0; i < 20 && result; i++)
        result = enqueue(queue, i, i * 7 % 20);
    for (int i = 0; i < 20 && result; i++)
        result = dequeue(queue, &value) && value == i * 3 % 20;

    result = result && enqueue(queue, 5, 1) && enqueue(queue, 2, 1) && enqueue(queue, 9, 0)
        && dequeue(queue, &value) && value == 9
        && dequeue(queue, &value) && value == 2
        && dequeue(queue, &value) && value == 5
        && !dequeue(queue, &value);
    freeQueue(queue);
    return result;
}

bool testReadMap(void)
{
    struct Map* map = mapFromText("3 2\n1 2 5\n2 3 7\n2\n3 1\n");
    bool result = map != NULL && map->graph->cityCount == 3 && map->stateCount == 2
        && hasRoad(map->graph, 0, 1, 5) && hasRoad(map->graph, 1, 0, 5)
        && hasRoad(map->graph, 1, 2, 7) && hasRoad(map->graph, 2, 1, 7)
        && !hasRoad(map->graph, 0, 2, 7)
        && map->owner[0] == 1 && map->owner[1] == -1 && map->owner[2] == 0;
    freeMap(map);

    return result
        && isWrongMap("")
        && isWrongMap("0 0\n1\n1\n")
        && isWrongMap("2 -1\n1\n1\n")
        && isWrongMap("2 1\n1 3 5\n1\n1\n")
        && isWrongMap("2 1\n0 1 5\n1\n1\n")
        && isWrongMap("2 1\n1 2 -5\n1\n1\n")
        && isWrongMap("2 1\n1 2 5\n0\n")
        && isWrongMap("2 1\n1 2 5\n3\n1 2 1\n")
        && isWrongMap("3 2\n1 2 5\n2 3 5\n2\n1 1\n")
        && isWrongMap("3 2\n1 2 5\n")
        && isWrongMap("3 2\n1 2 5\n2 3 5\n2\n1\n");
}

bool testDistributeCities(void)
{
    char* example = "8 10\n"
                    "1 2 4\n"
                    "1 3 2\n"
                    "2 3 5\n"
                    "2 4 10\n"
                    "3 5 3\n"
                    "4 5 4\n"
                    "4 6 11\n"
                    "5 7 6\n"
                    "6 7 2\n"
                    "6 8 3\n"
                    "2\n"
                    "1 6\n";
    char* nearestFromAnyCity = "6 5\n"
                               "1 2 1\n"
                               "1 3 5\n"
                               "2 4 2\n"
                               "6 5 1\n"
                               "5 3 1\n"
                               "2\n"
                               "1 6\n";
    char* shortestOfTwoRoads = "4 5\n"
                               "1 2 1\n"
                               "1 2 10\n"
                               "1 3 5\n"
                               "4 3 100\n"
                               "4 2 100\n"
                               "2\n"
                               "1 4\n";
    char* stuckInMiddle = "8 7\n"
                          "1 2 1\n"
                          "1 4 1\n"
                          "4 5 1\n"
                          "3 6 1\n"
                          "6 7 1\n"
                          "5 8 1\n"
                          "7 8 1\n"
                          "3\n"
                          "1 2 3\n";

    struct Map* disconnected = mapFromText("4 1\n1 2 1\n1\n1\n");
    bool result = disconnected != NULL && distributeCities(disconnected) == 2
        && disconnected->owner[1] == 0 && disconnected->owner[2] == -1 && disconnected->owner[3] == -1;
    freeMap(disconnected);

    return result
        && isDistribution(example, "Государство 1: 1 2 3 5\nГосударство 2: 4 6 7 8\n")
        && isDistribution(nearestFromAnyCity, "Государство 1: 1 2 4\nГосударство 2: 3 5 6\n")
        && isDistribution(shortestOfTwoRoads, "Государство 1: 1 2\nГосударство 2: 3 4\n")
        && isDistribution(stuckInMiddle, "Государство 1: 1 4 5 8\nГосударство 2: 2\nГосударство 3: 3 6 7\n")
        && isDistribution("3 2\n1 2 5\n3 2 5\n2\n3 1\n", "Государство 1: 2 3\nГосударство 2: 1\n")
        && isDistribution("5 4\n1 2 1\n2 3 1\n3 4 1\n4 5 1\n2\n1 2\n", "Государство 1: 1\nГосударство 2: 2 3 4 5\n")
        && isDistribution("1 0\n1\n1\n", "Государство 1: 1\n");
}

int main(int argc, char* argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        if (!testPriorityQueue() || !testReadMap() || !testDistributeCities()) {
            printf("Тесты не пройдены\n");
            return 1;
        }
        printf("Все тесты пройдены\n");
        return 0;
    }

    if (argc > 2) {
        printf("Использование: %s [cities.txt]\n", argv[0]);
        return 1;
    }

    char* path = argc == 2 ? argv[1] : "cities.txt";
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror(path);
        return 1;
    }
    struct Map* map = readMap(file);
    fclose(file);
    if (map == NULL) {
        printf("Не удалось прочитать карту из %s\n", path);
        return 1;
    }

    int count = distributeCities(map);
    bool isPrinted = false;
    if (count >= 0 && count < map->graph->cityCount)
        printf("Граф дорог несвязный, не до всех городов можно добраться от столиц\n");
    else if (count < 0 || !printStates(stdout, map))
        printf("Не хватает памяти\n");
    else
        isPrinted = true;

    freeMap(map);
    return isPrinted ? 0 : 1;
}
