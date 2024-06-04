#include <stdio.h>
#include <stdlib.h>

#include "compiler.h"
#include "token.h"
#include "scanner.h"

#ifdef DEBUG_PRINT_CODE
#include "debug.h"
#endif

Parser parser;
Chunk *currChunk;

/* Error Utils */
static void error(const char* errorMessage);
static void errorAt(Token *token, const char *errorMessage);
static void errorCurrent(const char *errorMessage);

static void parsePrecedence(Precedence precedence);
static ParseRule *getRule(TokenType tokenType);

static void expression();
static void number();
static void grouping();
static void unary();
static void binary();

static void emitByte(uint8_t instruction);
static void emitBytes(uint8_t a, uint8_t b);
static void emitReturn();

static void emitConstant(Value value);
static uint8_t makeConstant(Value value);

static void consume(TokenType type, const char *errorMessage);
static Chunk* getChunk();

static void endCompiler();
static void advance();

/* Parsing Rules */

ParseRule rules[] = {
  [TOKEN_LEFT_PAREN]    = {grouping, NULL,   PREC_NONE},
  [TOKEN_RIGHT_PAREN]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LEFT_BRACE]    = {NULL,     NULL,   PREC_NONE}, 
  [TOKEN_RIGHT_BRACE]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_COMMA]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_DOT]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_MINUS]         = {unary,    binary, PREC_TERM},
  [TOKEN_PLUS]          = {NULL,     binary, PREC_TERM},
  [TOKEN_SEMICOLON]     = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SLASH]         = {NULL,     binary, PREC_FACTOR},
  [TOKEN_STAR]          = {NULL,     binary, PREC_FACTOR},
  [TOKEN_BANG]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_BANG_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EQUAL_EQUAL]   = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER]       = {NULL,     NULL,   PREC_NONE},
  [TOKEN_GREATER_EQUAL] = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_LESS_EQUAL]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IDENTIFIER]    = {NULL,     NULL,   PREC_NONE},
  [TOKEN_STRING]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NUMBER]        = {number,   NULL,   PREC_NONE},
  [TOKEN_AND]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_CLASS]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ELSE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FALSE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FOR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_FUN]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_IF]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_NIL]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_OR]            = {NULL,     NULL,   PREC_NONE},
  [TOKEN_PRINT]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_RETURN]        = {NULL,     NULL,   PREC_NONE},
  [TOKEN_SUPER]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_THIS]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_TRUE]          = {NULL,     NULL,   PREC_NONE},
  [TOKEN_VAR]           = {NULL,     NULL,   PREC_NONE},
  [TOKEN_WHILE]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_ERROR]         = {NULL,     NULL,   PREC_NONE},
  [TOKEN_EOF]           = {NULL,     NULL,   PREC_NONE},
};

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
        if (parser.curr.type != TOKEN_ERROR)
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

static uint8_t makeConstant(Value value)
{
    int constant = addConstant(getChunk(), value);
    if (constant > UINT8_MAX)
    {
        error("Too many constants in one chunk.");
        return 0;
    }

    return (uint8_t)constant;
}

static void emitConstant(Value value)
{
    emitBytes(OP_CONSTANT, makeConstant(value));
}

static void unary()
{
    TokenType opType = parser.prev.type;

    /* Compile operand. */
    parsePrecedence(PREC_UNARY);

    switch (opType) 
    {
        case TOKEN_MINUS:
            emitByte(OP_NEGATE);
            break;

        default:
            return;
        }
}

static void binary()
{
    TokenType operator = parser.prev.type;
    ParseRule *rule = getRule(operator);
    parsePrecedence((Precedence)(rule->precedence + 1));

    switch (operator)
    {
        case TOKEN_PLUS: emitByte(OP_ADD); break;
        case TOKEN_MINUS: emitByte(OP_SUBTRACT); break;
        case TOKEN_STAR: emitByte(OP_MULTIPLY); break;
        case TOKEN_SLASH: emitByte(OP_DIVIDE); break;
        default: break;
    }
}

static void parsePrecedence(Precedence precedence)
{
    advance();
    ParseFn prefixRule = getRule(parser.prev.type)->prefix;
    if (prefixRule == NULL)
    {
        error("Expect expression.");
        return;
    }

    prefixRule();

    while (precedence <= getRule(parser.curr.type)->precedence)
    {
        advance();
        ParseFn infixRule = getRule(parser.prev.type)->infix;
        infixRule();
    }
}

static void endCompiler()
{
#ifdef DEBUG_PRINT_CODE
    if (!parser.hadError)
        disassembleChunk(getChunk(), "code");
#endif
    emitReturn();
}

static Chunk* getChunk()
{
    return currChunk;
}

/*  */

static void expression()
{
    parsePrecedence(PREC_ASSIGNMENT);
}

static void number() 
{
    double value = strtod(parser.prev.start, NULL);
    emitConstant(value);
}

static void grouping()
{
    expression();
    consume(TOKEN_RIGHT_PAREN, "Expect ')' after expression.");
}

static ParseRule* getRule(TokenType tokenType)
{
    return &rules[tokenType];
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
