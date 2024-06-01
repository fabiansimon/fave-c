#include <stdio.h>

#include "compiler.h"
#include "token.h"
#include "scanner.h"

Parser parser;

/* Error Utils */
static void error(const char* errorMessage);
static void errorAt(Token *token, const char *errorMessage);
static void errorCurrent(const char *errorMessage);
static void consume(TokenType type, const char* errorMessage);

static void advance();

bool compile(const char *src, Chunk *chunk)
{ 
    initScanner(src);

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    consume(TOKEN_EOF, "Expect end of expression.");
    return !parser.hadError;
}

static void advance()
{
    parser.prev = parser.curr;

    for (;;)
    {
        parser.curr = scanToken();
        if (parser.curr.type == TOKEN_ERROR)
            break;

        errorCurrent(parser.curr.start);
    }
}

static void consume(TokenType type, const char* errorMessage)
{
    if (parser.curr.type != type)
    {
        errorCurrent(errorMessage);
        return;
    }

    advance();
}

static void errorCurrent(const char *errorMessage)
{
    errorAt(&parser.curr, errorMessage);
}

static void errorAt(Token *token, const char *errorMessage)
{
    if (parser.panicMode)
        return;
        
    parser.panicMode = true;
    fprintf(stderr, "[line %d] Error", token->line);

    if (token->type == TOKEN_EOF)
    {
        fprintf(stderr, " at end");
    } else if (token->type != TOKEN_ERROR)
    {
        fprintf(stderr, " at '%.*s'", token->length, token->start);
    }

    fprintf(stderr, ": %s\n", errorMessage);
    parser.hadError = true;
}

static void error(const char *errorMessage)
{
    errorAt(&parser.prev, errorMessage);
}

