#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"

int main()
{
    initVM();

    Chunk chunk;

    initChunk(&chunk);

    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT, 1);
    writeChunk(&chunk, constant, 1);

    constant = addConstant(&chunk, 3.4);
    writeChunk(&chunk, OP_CONSTANT, 2);
    writeChunk(&chunk, constant, 2);

    writeChunk(&chunk, OP_ADD, 3);

    constant = addConstant(&chunk, 5.6);
    writeChunk(&chunk, OP_CONSTANT, 4);
    writeChunk(&chunk, constant, 24);

    writeChunk(&chunk, OP_DIVIDE, 5);

    writeChunk(&chunk, OP_NEGATE, 6);
    writeChunk(&chunk, OP_RETURN, 6);

    interpret(&chunk);

    // disassembleChunk(&chunk, "Test Chunk");

    freeChunk(&chunk);
    return 0;
}