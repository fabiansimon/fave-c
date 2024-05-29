#include "vm.h"
#include "debug.h"

VM vm;

static InterpretResult run();
static void resetStack();

void initVM()
{
    resetStack();
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

uint8_t readByte()
{
    return *vm.ip++;
}

Value readConstant()
{
    return vm.chunk->constants.values[readByte()];
}

void push(Value value)
{
    *vm.stackTop = value;
    vm.stackTop++;
}

Value pop()
{
    vm.stackTop--;
    return *vm.stackTop;
}

static InterpretResult run()
{

    InterpretResult result;
    for (;;)
    {

#ifdef DEBUG_TRACE_EXECUTION
        int offset = (int)(vm.ip - vm.chunk->code);
        disassembleInstruction(vm.chunk, &offset);
        printf("          ");
        for (Value *slot = vm.stack; slot < vm.stackTop; ++slot)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
#endif

        uint8_t instruction;
        switch (instruction = readByte())
        {
        case OP_CONSTANT:
        {
            Value constant = readConstant();
            push(constant);
            break;
        }
        case OP_RETURN:
        {
            printValue(pop());
            printf("\n");
            return INTERPRET_OK;
        }
        }
    }

    result = INTERPRET_COMPILE_ERROR;
    return result;
}

static void resetStack()
{
    vm.stackTop = vm.stack;
}