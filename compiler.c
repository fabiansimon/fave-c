#include <stdio.h>

#include "compiler.h"
#include "token.h"
#include "scanner.h"

Parser parser;
Chunk *currChunk;

/* Error Utils */
static void error(const char* errorMessage);
static void errorAt(Token *token, const char *errorMessage);
static void errorCurrent(const char *errorMessage);

static void emitByte(uint8_t instruction);
static void emitBytes(uint8_t a, uint8_t b);
static void emitReturn();

static void consume(TokenType type, const char *errorMessage);
static Chunk* getChunk();

static void endCompiler();

static void advance();

bool compile(const char *src, Chunk *chunk)
{ 
    initScanner(src);
    currChunk = chunk;

    parser.hadError = false;
    parser.panicMode = false;

    advance();
    consume(TOKEN_EOF, "Expect end of expression.");
    endCompiler();
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

static void emitByte(uint8_t instruction)
{
    writeChunk(getChunk(), instruction, parser.prev.line);
}

static void emitBytes(uint8_t a, uint8_t b)
{
    emitByte(a);
    emitByte(b);
}

static void emitReturn()
{
    emitByte(OP_RETURN);
}

static void endCompiler()
{
    emitReturn();
}

static Chunk* getChunk()
{
    return currChunk;
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

