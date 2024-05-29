#include <stdio.h>

#include "chunk.h"
#include "debug.h"

static void simpleInstruction(const char *name, int *offset);
static void constantInstruction(const char *name, Chunk *chunk, int *offset);

void disassembleChunk(Chunk *chunk, const char *name)
{
    printf("\n=== Chunk: %s ===\n\n", name);

    int offset = 0;
    while (offset < chunk->count)
    {
        disassembleInstruction(chunk, &offset);
    }
}

void disassembleInstruction(Chunk *chunk, int *offset)
{
    printf("%04d ", *offset);

    int line = getLine(&chunk->lines, offset);
    printf("%4d ", line);
    // if (*offset > 0 && chunk->lines[*offset] == chunk->lines[*offset-1])
    //     printf("   | ");
    // else
    //     printf("%4d ", chunk->lines[*offset]);

    uint8_t instruction = chunk->code[*offset];
    switch (instruction)
    {
    case OP_RETURN:
        simpleInstruction("OP_RETURN", offset);
        return;

    case OP_CONSTANT:
        constantInstruction("CONSTANT", chunk, offset);
        return;

    default:
        printf("Unknown instruction %d\n", instruction);
        (*offset)++;
        return;
    }
}

static void simpleInstruction(const char *name, int *offset)
{
    printf("%s\n", name);
    (*offset)++;
}

static void constantInstruction(const char *name, Chunk *chunk, int *offset)
{
    uint8_t constant = chunk->code[*offset + 1];
    printf("%-16s %4d '", name, constant);
    printValue(chunk->constants.values[constant]);
    printf("'\n");
    (*offset) += 2;
}