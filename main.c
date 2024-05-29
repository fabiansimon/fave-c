#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

static void repl();
static void runFile(const char *path);

int main(int argc, const char *argv[])
{

    initVM();

    if (argc != 1 && argc != 2)
    {
        fprintf(stderr, "Usage: fave [path]\n");
        exit(64);
    }

    if (argc == 1)
        repl();
    else
        runFile(argv[1]);

    freeVM();

    return 0;
}

static void repl()
{
    char buffer[1024];
    for (;;)
    {
        printf("> ");
        if (!fgets(buffer, sizeof(buffer), stdin))
        {
            printf("\n");
            break;
        }

        interpret(line) :
    }
}

static char *readFile(const char *path)
{
    FILE *file = fopen(path, "rb");

    if (file == NULL)
    {
        fprintf(stderr, "File could not be open \"%s\".\n", path);
        exit(74);
    }

    fseek(file, 0L, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char *buffer = (char *)malloc(size + 1);
    if (buffer == NULL)
    {
        fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
        exit(74);
    }

    size_t bytesRead = fread(buffer, sizeof(char), size, file);
    if (bytesRead < fileSize)
    {
        fprintf(stderr, "Could not read file \"%s\".\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}

static void runFile(const char *path)
{
    char *src = readFile(path);
    InterpretResult res = interpret(src);
    free(src);

    if (res == INTERPRET_COMPILE_ERROR)
        exit(65);
    if (res == INTERPRET_RUNTIME_ERROR)
        exit(70);
}