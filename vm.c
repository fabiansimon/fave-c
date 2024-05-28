#include "vm.h"

VM vm;

static InterpretResult run();

void initVM()
{

}

void freeVM() 
{

}

InterpretResult interpret(Chunk *chunk) 
{
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

static InterpretResult run() 
{
    InterpretResult result; 
    for (;;)
}