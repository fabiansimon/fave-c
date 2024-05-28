#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main()
{
    initVM();

    Chunk chunk;

    initChunk(&chunk);

    int constant =  addConstant(&chunk, 3.1);
    writeChunk(&chunk, OP_CONSTANT, 1);
    writeChunk(&chunk, constant, 1);

    constant =  addConstant(&chunk, 4.3);
    writeChunk(&chunk, OP_CONSTANT, 1);
    writeChunk(&chunk, constant, 1);

    constant =  addConstant(&chunk, 10.2);
    writeChunk(&chunk, OP_CONSTANT, 3);
    writeChunk(&chunk, constant, 3);

    writeChunk(&chunk, OP_RETURN, 10);

    disassembleChunk(&chunk, "Test Chunk");
    
    freeChunk(&chunk);
    return 0;
}