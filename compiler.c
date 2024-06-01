#include <stdio.h>

#include "compiler.h"
#include "token.h"
#include "scanner.h"

void compile(const char *src)
{
    // var i = 0;
    initScanner(src);
    int line = -1;
    for (int i = 0; i < 10; ++i)
    {
        Token token = scanToken();
        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF)
            break;
    }
}