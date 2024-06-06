#include <stdarg.h>

#include "vm.h"
#include "debug.h"
#include "compiler.h"

#define BINARY_OPERATION(type, op)                                  \
    do                                                              \
    {                                                               \
        if (!IS_NUMBER(peek(0)) || !IS_NUMBER(peek(1)))             \
        {                                                           \
            runtimeError("Operands must be numbers.");              \
            return INTERPRET_RUNTIME_ERROR;                         \
        }                                                           \
                                                                    \
        double b = AS_NUMBER(pop());                                \
        double a = AS_NUMBER(pop());                                \
        push(type(a op b));                                    \
    } while (false)

/* Global Virtual Machine instance */
VM vm;

static InterpretResult run();
static Value peek(int skip);
static void resetStack();
static bool isFalsey(Value value);
static void runtimeError(const char *format, ...);

void initVM()
{
    resetStack();
}

void freeVM()
{
}

InterpretResult interpret(const char *src)
{
    Chunk chunk;
    initChunk(&chunk);

    if (!compile(src, &chunk))
    {
        freeChunk(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    InterpretResult result = run();

    freeChunk(&chunk);
    return result;
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

static Value peek(int skip)
{
    return vm.stackTop[-1 - skip];
}

static bool isFalsey(Value value)
{
    if (IS_NIL(value))
        return true;

    return IS_BOOL(value) && !AS_BOOL(value);
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
            if (!IS_NUMBER(peek(0))) 
            {
                runtimeError("Operand must be a number.");
                return INTERPRET_RUNTIME_ERROR;
            }

            push(NUMBER_VAL(-AS_NUMBER(pop())));
            break;

        case OP_ADD:
            BINARY_OPERATION(NUMBER_VAL, +);
            break;
        case OP_SUBTRACT:
            BINARY_OPERATION(NUMBER_VAL, -);
            break;
        case OP_MULTIPLY:
            BINARY_OPERATION(NUMBER_VAL, *);
            break;
        case OP_DIVIDE:
            BINARY_OPERATION(NUMBER_VAL, /);
            break;

        case OP_CONSTANT:
        {
            Value constant = readConstant();
            push(constant);
            break;
        }

        case OP_NOT:
            push(BOOL_VAL(isFalsey(pop())));
            break;

        case OP_NIL:
            push(NIL_VAL);
            break;

        case OP_TRUE:
            push(BOOL_VAL(true));
            break;

        case OP_FALSE:
            push(BOOL_VAL(false));
            break;

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

static void runtimeError(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fputs("\n", stderr);

    /* <-- Replace with code that works with custom line implementation
    size_t instruction = vm.ip - vm.chunk->code - 1;
    */
    int line = 1;
    fprintf(stderr, "[line %d] in script\n", line);
    resetStack();
}