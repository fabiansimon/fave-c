#include "common.h"
#include "chunk.h"

int main()
{
    Chunk chunk;
    initChunk(&chunk);
    printf("%d\n", chunk.count);
    writeChunk(&chunk, OP_RETURN);
    printf("%d\n", chunk.count);
    freeChunk(&chunk);
    printf("%d\n", chunk.count);
    return 0;
}