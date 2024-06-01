#ifndef COMPILER_H
#define COMPILER_H

#include "vm.h"
#include "token.h"

typedef struct
{
    Token prev;
    Token curr;
    bool panicMode; 
    bool hadError;
} Parser;

bool compile(const char *src, Chunk *chunk);

#endif