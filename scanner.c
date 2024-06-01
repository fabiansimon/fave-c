#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "scanner.h"
#include "token.h"

Scanner scanner;

/* Token generators */
static Token createToken(TokenType type);
static Token numberToken();
static Token stringToken();
static Token identifierToken();
static Token errorToken(const char* errorMessage);

/* Utils functions */
static char advance(int steps);
static bool isEnd();
static char peekForward(int skip);
static char peek();

static bool isMatch(char expected);
static bool isDigit(char character);
static bool isChar(char character);

static void skipWhitespace();
static void skipComment();

static bool checkKeyword(int start, int length, const char *rest);
static TokenType identifyType();

void initScanner(const char *src)
{
    scanner.left = src;
    scanner.right = src;
    scanner.line = 1;
}

Token scanToken()
{
    skipWhitespace();
    scanner.left = scanner.right;

    if (isEnd()) 
        return createToken(TOKEN_EOF);
    
    char c = advance(1);

    if (isDigit(c))
        return numberToken();

    if (isChar(c))
        return identifierToken();

    switch (c) 
    {
        case '(': return createToken(TOKEN_LEFT_PAREN);
        case ')': return createToken(TOKEN_RIGHT_PAREN);
        case '{': return createToken(TOKEN_LEFT_BRACE);
        case '}': return createToken(TOKEN_RIGHT_BRACE);
        case ';': return createToken(TOKEN_SEMICOLON);
        case ',': return createToken(TOKEN_COMMA);
        case '.': return createToken(TOKEN_DOT);
        case '-': return createToken(TOKEN_MINUS);
        case '+': return createToken(TOKEN_PLUS);
        case '/': return createToken(TOKEN_SLASH);
        case '*': return createToken(TOKEN_STAR);
        case '"':
            return stringToken();
        case '!':
            return createToken(isMatch('=') ? TOKEN_BANG_EQUAL : TOKEN_BANG);
        case '=':
            return createToken(isMatch('=') ? TOKEN_EQUAL_EQUAL : TOKEN_EQUAL);
        case '<':
            return createToken(isMatch('=') ? TOKEN_LESS_EQUAL : TOKEN_LESS);
        case '>':
            return createToken(isMatch('=') ? TOKEN_GREATER_EQUAL : TOKEN_GREATER);
        }

    return errorToken("Unexpected character.");
}

static bool isEnd()
{
    return *scanner.right == '\0';
}

static bool isMatch(char expected)
{
    if (isEnd() || *scanner.right != expected)
        return false;

    scanner.right++;
    return true;
}

static bool isDigit(char character)
{
    return character >= '0' && character <= '9';
}

static bool isChar(char character)
{
    return (character >= 'a' && character <= 'z') || (character >= 'A' && character <= 'Z') || (character == '_');
}

static char advance(int steps) 
{
    while (steps > 0 && !isEnd()) {
        scanner.right++;
        steps--;
    }
    
    return scanner.right[-1];
}

static char peek()
{
    return *scanner.right;
}


static char peekForward(int skip)
{
    const char *curr = scanner.right;
    for (int i = 0; i < skip; ++i, ++curr)
    {   
        if (*curr == '\0')
            return '\0';
    };

    return *curr;
}

static void skipWhitespace()
{
    for (;;)
    {
        char c = peek();
        switch(c)
        {
            case ' ':
            case '\r':
            case '\t':
            case '\n':
            {
                if (c == '\n')
                    scanner.line++;

                advance(1);
                break;
            }

            case '/':
                skipComment();
                break;
            default:
                return;
            }
    }
}

static void skipComment() {
    const char next = peekForward(1);
    if (next != '/' && next != '*')
        return;

    bool multiline = next == '*';
    advance(2);

    while (!isEnd()) {
        if (multiline) {
            if (peekForward(1) == '*' && peekForward(2) == '/') {
                advance(3);
                scanner.line++;
                return;
            } else if (peekForward(1) == '\n') {
                scanner.line++;
            }
        } else {
            if (peekForward(1) == '\n') {
                advance(1);
                return;
            }
        }

        advance(1);
    }
}

static Token createToken(TokenType type)
{
    Token token;
    token.type = type;
    token.start = scanner.left;
    token.length = (int)(scanner.right - scanner.left);
    token.line = scanner.line;
    return token;
}

static Token stringToken()
{
    char curr = peek();
    while (!isEnd() && curr != '"')
    {   
        if (curr == '\n')
            scanner.line++;

        advance(1);
    }

    if (isEnd())
        return errorToken("Unterminated string.");

    advance(1); // closing "
    return createToken(TOKEN_STRING);
}

static Token numberToken()
{
    while (isDigit(peek()))
        advance(1);

    if (peek() == '.' && isDigit(peekForward(1))) // check for decimal number
    {
        advance(1);
        while (isDigit(peek()))
            advance(1);
    }

    return createToken(TOKEN_NUMBER);
}

static TokenType identifyType() 
{
    switch (scanner.left[0]) 
    {
        case 'a': return checkKeyword(1, 2, "nd") ? TOKEN_AND : TOKEN_IDENTIFIER;
        case 'c': return checkKeyword(1, 4, "lass") ? TOKEN_CLASS : TOKEN_IDENTIFIER;
        case 'e': return checkKeyword(1, 3, "lse") ? TOKEN_ELSE : TOKEN_IDENTIFIER;
        case 'i': return checkKeyword(1, 1, "f") ? TOKEN_IF : TOKEN_IDENTIFIER;
        case 'n': return checkKeyword(1, 2, "il") ? TOKEN_NIL : TOKEN_IDENTIFIER;
        case 'o': return checkKeyword(1, 1, "r") ? TOKEN_OR : TOKEN_IDENTIFIER;
        case 'p': return checkKeyword(1, 4, "rint") ? TOKEN_PRINT : TOKEN_IDENTIFIER;
        case 'r': return checkKeyword(1, 5, "eturn") ? TOKEN_RETURN : TOKEN_IDENTIFIER;
        case 's': return checkKeyword(1, 4, "uper") ? TOKEN_SUPER : TOKEN_IDENTIFIER;
        case 'v': return checkKeyword(1, 2, "ar") ? TOKEN_VAR : TOKEN_IDENTIFIER;
        case 'w': return checkKeyword(1, 4, "hile") ? TOKEN_WHILE : TOKEN_IDENTIFIER;
        case 'f': 
        {
            if (scanner.right - scanner.left > 1)
            {
                switch (scanner.left[1])
                {
                    case 'a': return checkKeyword(2, 3, "lse") ? TOKEN_FALSE : TOKEN_IDENTIFIER;
                    case 'o': return checkKeyword(2, 1, "r") ? TOKEN_FOR : TOKEN_IDENTIFIER;
                    case 'u': return checkKeyword(2, 2, "un") ? TOKEN_FUN : TOKEN_IDENTIFIER;
                }
            }
            break;
        }
        case 't':
        {
            if (scanner.right - scanner.left > 1)
            {
                switch (scanner.left[1])
                {
                    case 'h': return checkKeyword(2, 2, "is") ? TOKEN_THIS : TOKEN_IDENTIFIER;
                    case 'r': return checkKeyword(2, 2, "ue") ? TOKEN_TRUE : TOKEN_IDENTIFIER;
                    }
            }
        }
        default:
            return TOKEN_IDENTIFIER;
        }

    return TOKEN_IDENTIFIER;
}

static bool checkKeyword(int start, int length, const char *rest)
{
    if (scanner.right - scanner.left == start + length &&
        memcmp(scanner.left + start, rest, length) == 0)
        return true;

    return false;
}

static Token identifierToken()
{
    char curr = peek();
    while (isChar(curr) || isDigit(curr)) 
    {
        advance(1);
        curr = peek();
    }

    return createToken(identifyType());
}

static Token errorToken(const char* errorMessage)
{
    Token token;
    token.type = TOKEN_ERROR;
    token.start = errorMessage;
    token.length = (int)strlen(errorMessage);
    token.line = scanner.line;
    return token;
}