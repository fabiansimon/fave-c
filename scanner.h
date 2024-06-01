#ifndef SCANNER_H
#define SCANNER_H

#include "token.h"

typedef struct
{
    const char *left;
    const char *right;
    int line;
} Scanner;

void initScanner(const char *src);
Token scanToken();

#endif