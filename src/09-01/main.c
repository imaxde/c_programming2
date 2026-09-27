#include "graph.h"
#include "queue.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Graph* graphFromText(char* text)
{
    FILE* file = tmpfile();
    if (file == NULL)
        return NULL;

    struct Graph* graph = NULL;
    if (fputs(text, file) != EOF && fseek(file, 0, SEEK_SET) == 0)
        graph = readGraph(file);
    fclose(file);
    return graph;
}

bool isBfsOrder(char* text, int start, const int* expected, int expectedCount)
{
    struct Graph* graph = graphFromText(text);
    if (graph == NULL)
        return false;

    int* order = calloc(graph->vertexCount, sizeof(int));
    int count = order == NULL ? -1 : breadthFirstSearch(graph, start, order);
    bool result = count == expectedCount;
    for (int i = 0; i < expectedCount && result; i++)
        result = order[i] == expected[i];

    free(order);
    freeGraph(graph);
    return result;
}

bool testQueue(void)
{
    struct Queue* queue = createQueue();
    if (queue == NULL)
        return false;

    int value = 0;
    bool result = isEmpty(queue) && !dequeue(queue, &value)
        && enqueue(queue, 1) && enqueue(queue, 2) && !isEmpty(queue)
        && dequeue(queue, &value) && value == 1
        && enqueue(queue, 3)
        && dequeue(queue, &value) && value == 2
        && dequeue(queue, &value) && value == 3
        && isEmpty(queue) && !dequeue(queue, &value);
    freeQueue(queue);
    return result;
}

bool testReadGraph(void)
{
    struct Graph* graph = graphFromText("3\n0 1 0\n0 0 5\n1 0 0\n");
    bool result = graph != NULL && graph->vertexCount == 3
        && hasEdge(graph, 0, 1) && !hasEdge(graph, 1, 0)
        && hasEdge(graph, 1, 2) && hasEdge(graph, 2, 0) && !hasEdge(graph, 0, 0);
    freeGraph(graph);

    struct Graph* shortMatrix = graphFromText("3\n0 1 0\n0 0\n");
    struct Graph* noVertices = graphFromText("0\n");
    struct Graph* notNumber = graphFromText("abc");
    result = result && shortMatrix == NULL && noVertices == NULL && notNumber == NULL;
    freeGraph(shortMatrix);
    freeGraph(noVertices);
    freeGraph(notNumber);
    return result;
}

bool testBfs(void)
{
    char* lectureGraph = "5\n"
                         "0 1 0 0 1\n"
                         "1 0 1 1 1\n"
                         "0 1 0 1 0\n"
                         "0 1 1 0 1\n"
                         "1 1 0 1 0\n";
    int fromZero[] = { 0, 1, 4, 2, 3 };
    int fromTwo[] = { 2, 1, 3, 0, 4 };

    char* directedGraph = "4\n"
                          "0 1 0 0\n"
                          "0 0 0 0\n"
                          "0 1 0 0\n"
                          "0 0 0 0\n";
    int directedFromZero[] = { 0, 1 };
    int directedFromTwo[] = { 2, 1 };
    int isolated[] = { 3 };

    char* levelsGraph = "4\n"
                        "0 1 1 0\n"
                        "0 0 0 1\n"
                        "0 0 0 0\n"
                        "0 0 0 0\n";
    int byLevels[] = { 0, 1, 2, 3 };

    int loop[] = { 0 };

    return isBfsOrder(lectureGraph, 0, fromZero, 5)
        && isBfsOrder(lectureGraph, 2, fromTwo, 5)
        && isBfsOrder(directedGraph, 0, directedFromZero, 2)
        && isBfsOrder(directedGraph, 2, directedFromTwo, 2)
        && isBfsOrder(directedGraph, 3, isolated, 1)
        && isBfsOrder(levelsGraph, 0, byLevels, 4)
        && isBfsOrder("1\n1\n", 0, loop, 1);
}

int main(int argc, char* argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        if (!testQueue() || !testReadGraph() || !testBfs()) {
            printf("Тесты не пройдены\n");
            return 1;
        }
        printf("Все тесты пройдены\n");
        return 0;
    }

    if (argc < 2 || argc > 3) {
        printf("Использование: %s <вершина> [graph.txt]\n", argv[0]);
        return 1;
    }

    int start = 0;
    char extra = 0;
    if (sscanf(argv[1], "%d%c", &start, &extra) != 1) {
        printf("Вершина должна быть числом\n");
        return 1;
    }

    char* path = argc == 3 ? argv[2] : "graph.txt";
    FILE* file = fopen(path, "r");
    if (file == NULL) {
        perror(path);
        return 1;
    }
    struct Graph* graph = readGraph(file);
    fclose(file);
    if (graph == NULL) {
        printf("Не удалось прочитать граф из %s\n", path);
        return 1;
    }

    if (start < 0 || start >= graph->vertexCount) {
        printf("Нет вершины %d, есть от 0 до %d\n", start, graph->vertexCount - 1);
        freeGraph(graph);
        return 1;
    }

    int* order = calloc(graph->vertexCount, sizeof(int));
    int count = order == NULL ? -1 : breadthFirstSearch(graph, start, order);
    if (count < 0) {
        printf("Не хватает памяти\n");
        free(order);
        freeGraph(graph);
        return 1;
    }

    for (int i = 0; i < count; i++) {
        if (i > 0)
            printf(" ");
        printf("%d", order[i]);
    }
    printf("\n");

    free(order);
    freeGraph(graph);
    return 0;
}
