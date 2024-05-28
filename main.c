#include "common.h"
#include "chunk.h"
#include "debug.h"

int main()
{
    Chunk chunk;

    printf("%d\n", OP_RETURN);
    printf("%d\n", OP_CONSTANT);
    initChunk(&chunk);
    
    writeChunk(&chunk, OP_RETURN, 1);

    int constant =  addConstant(&chunk, 1.1);
    writeChunk(&chunk, OP_CONSTANT, 1);
    writeChunk(&chunk, constant, 1);

    constant =  addConstant(&chunk, 4.3);
    writeChunk(&chunk, OP_CONSTANT, 4);
    writeChunk(&chunk, constant, 5);

    disassembleChunk(&chunk, "Test Chunk");
    freeChunk(&chunk);
    return 0;
}