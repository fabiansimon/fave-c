#include "chunk.h"
#include "memory.h"

void initChunk(Chunk *chunk)
{
    chunk->code = NULL;
    chunk->capacity = 0;
    chunk->count = 0;

    initLineArray(&chunk->lines);
    initValueArray(&chunk->constants);
}

void writeChunk(Chunk *chunk, uint8_t instruction, int line)
{
    if (chunk->count + 1 >= chunk->capacity)
    {
        int oldCapacity = chunk->capacity;
        chunk->capacity = GROW_CAPACITY(oldCapacity);
        chunk->code = GROW_ARRAY(uint8_t, chunk->code, oldCapacity, chunk->capacity);
    }

    writeLineArray(&chunk->lines, line);

    chunk->code[chunk->count] = instruction;
    chunk->count++;
}

void freeChunk(Chunk *chunk)
{
    FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    
    freeValueArray(&chunk->constants);
    freeLineArray(&chunk->lines);

    initChunk(chunk);
}

int addConstant(Chunk *chunk, Value value)
{
    writeValueArray(&chunk->constants, value);
    return chunk->constants.count - 1;
}
