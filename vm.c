#include "vm.h"
#include "debug.h"
#include "compiler.h"

VM vm;

#define BINARY_OPERATION(op) \
    do                       \
    {                        \
        double b = pop();    \
        double a = pop();    \
        push(a op b);        \
    } while (false)

static InterpretResult run();
static void resetStack();

void initVM()
{
    resetStack();
}

void freeVM()
{
}

InterpretResult interpret(const char *src)
{
    compile(src);
    return INTERPRET_OK;
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
        for (Value *slot = vm.stack; slot < vm.stackTop; slot++)
        {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        printf("\n");
#endif

        uint8_t instruction;
        switch (instruction = readByte())
        {
        case OP_NEGATE:
            push(-pop());
            break;

        case OP_ADD:
            BINARY_OPERATION(+);
            break;
        case OP_SUBTRACT:
            BINARY_OPERATION(-);
            break;
        case OP_MULTIPLY:
            BINARY_OPERATION(*);
            break;
        case OP_DIVIDE:
            BINARY_OPERATION(/);
            break;

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