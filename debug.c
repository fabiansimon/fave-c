#include <stdio.h>

#include "chunk.h"
#include "debug.h"

static void disassembleInstruction(Chunk *chunk, int *offset);
static void simpleInstruction(const char *name, int *offset);

void disassembleChunk(Chunk *chunk, const char *name)
{
    printf("=== Chunk: %s ===\n", name);

    int offset = 0;
    while (offset < chunk->count)
    {
        disassembleInstruction(chunk, &offset);
    }
}


static void simpleInstruction(const char *name, int *offset)
{
    printf("%s\n", name);
    (*offset)++;
}


static void disassembleInstruction(Chunk *chunk, int *offset)
{
    printf("%04d ", *offset);

    uint8_t instruction = chunk->code[*offset];
    switch (instruction) {
        case OP_RETURN:
            simpleInstruction("OP_RETURN", offset);
            break;

        default:
            printf("Unknown instruction %d\n", instruction);
            (*offset)++;
            return;
        }
}
