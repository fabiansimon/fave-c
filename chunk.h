#ifndef CHUNK_H
#define CHUNK_H

#include "common.h"
#include "value.h"
#include "line.h"

typedef enum
{
    OP_CONSTANT,
    OP_RETURN,
} OpCode;

typedef struct
{
    uint8_t *code;
    int count;
    int capacity;

    LineArray lines;

    ValueArray constants;
} Chunk;

void initChunk(Chunk *chunk);
void writeChunk(Chunk *chunk, uint8_t instruction, int line);
void freeChunk(Chunk *chunk);
int addConstant(Chunk *chunk, Value value);

#endif
