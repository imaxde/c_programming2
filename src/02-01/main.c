#include "prettyCsv.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool hasContent(FILE* file, char* expected)
{
    if (fseek(file, 0, SEEK_SET) != 0)
        return false;

    for (char* current = expected; *current != '\0'; current++) {
        if (fgetc(file) != (unsigned char)*current)
            return false;
    }
    return fgetc(file) == EOF;
}

bool isRenderedAs(char* csv, char* expected)
{
    FILE* input = tmpfile();
    FILE* output = tmpfile();
    bool result = false;

    if (input != NULL && output != NULL && fputs(csv, input) != EOF && fseek(input, 0, SEEK_SET) == 0) {
        struct Table table;
        if (readCSV(input, &table)) {
            result = writeTable(output, &table) && hasContent(output, expected);
            freeTable(&table);
        }
    }

    if (input != NULL)
        fclose(input);
    if (output != NULL)
        fclose(output);
    return result;
}

bool testIsNumber(void)
{
    return isNumber("123") && isNumber("28.7") && isNumber("-3") && isNumber("+0.5")
        && isNumber(".5") && isNumber("7.") && isNumber("1e10") && isNumber("2.5E-3")
        && !isNumber("") && !isNumber("text") && !isNumber("12a") && !isNumber("1.2.3")
        && !isNumber("-") && !isNumber(".") && !isNumber("e5") && !isNumber("1e")
        && !isNumber(" 12") && !isNumber("inf") && !isNumber("0x1F");
}

bool testExampleTable(void)
{
    return isRenderedAs(
        "Test field 1,Test field 2\n"
        "test,123\n"
        "long string test!,28.7\n"
        "other text,3\n",
        "+===================+==============+\n"
        "| Test field 1      | Test field 2 |\n"
        "+===================+==============+\n"
        "| test              |          123 |\n"
        "+-------------------+--------------+\n"
        "| long string test! |         28.7 |\n"
        "+-------------------+--------------+\n"
        "| other text        |            3 |\n"
        "+-------------------+--------------+\n");
}

bool testHeaderAlignedLeft(void)
{
    return isRenderedAs(
        "1,22\n"
        "333,4\n",
        "+=====+====+\n"
        "| 1   | 22 |\n"
        "+=====+====+\n"
        "| 333 |  4 |\n"
        "+-----+----+\n");
}

bool testRowsOfDifferentLength(void)
{
    return isRenderedAs(
        "a,b\n"
        "1\n"
        "x,,zz\n"
        "c,d,e,f\n",
        "+===+===+====+===+\n"
        "| a | b |    |   |\n"
        "+===+===+====+===+\n"
        "| 1 |   |    |   |\n"
        "+---+---+----+---+\n"
        "| x |   | zz |   |\n"
        "+---+---+----+---+\n"
        "| c | d | e  | f |\n"
        "+---+---+----+---+\n");
}

bool testLineEndings(void)
{
    return isRenderedAs(
        "name,value\r\n"
        "\r\n"
        "x,-1.5\r"
        "y,2",
        "+======+=======+\n"
        "| name | value |\n"
        "+======+=======+\n"
        "| x    |  -1.5 |\n"
        "+------+-------+\n"
        "| y    |     2 |\n"
        "+------+-------+\n");
}

bool testHeaderOnly(void)
{
    return isRenderedAs(
        "id,name\n",
        "+====+======+\n"
        "| id | name |\n"
        "+====+======+\n");
}

bool testEmptyInput(void)
{
    return isRenderedAs("", "") && isRenderedAs("\n\n", "");
}

bool convertFile(char* inputPath, char* outputPath)
{
    FILE* input = fopen(inputPath, "r");
    if (input == NULL) {
        perror(inputPath);
        return false;
    }

    struct Table table;
    bool isRead = readCSV(input, &table);
    fclose(input);
    if (!isRead) {
        fprintf(stderr, "Не удалось прочитать \"%s\".\n", inputPath);
        return false;
    }

    FILE* output = fopen(outputPath, "w");
    if (output == NULL) {
        perror(outputPath);
        freeTable(&table);
        return false;
    }

    bool isWritten = writeTable(output, &table);
    bool isClosed = fclose(output) == 0;
    freeTable(&table);
    if (!isWritten || !isClosed) {
        fprintf(stderr, "Не удалось записать \"%s\".\n", outputPath);
        return false;
    }
    return true;
}

int main(int argc, char* argv[])
{
    if (argc == 2 && strcmp(argv[1], "--test") == 0) {
        if (!testIsNumber() || !testExampleTable() || !testHeaderAlignedLeft()
            || !testRowsOfDifferentLength() || !testLineEndings() || !testHeaderOnly()
            || !testEmptyInput()) {
            printf("Тесты не пройдены.\n");
            return 1;
        }
        printf("Все тесты пройдены.\n");
        return 0;
    }

    char* inputPath = "input.csv";
    char* outputPath = "output.txt";

    if (argc == 3) {
        inputPath = argv[1];
        outputPath = argv[2];
    } else if (argc != 1) {
        fprintf(stderr, "Использование: %s [input.csv output.txt]\n", argv[0]);
        fprintf(stderr, "Запуск тестов: %s --test\n", argv[0]);
        return 1;
    }

    return convertFile(inputPath, outputPath) ? 0 : 1;
}
