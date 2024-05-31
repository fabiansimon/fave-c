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
static char advance();
static bool isEnd();
static char peekForward();
static char peek();

static bool isMatch(char expected);
static bool isDigit(char character);
static bool isChar(char character);

static void skipWhitespace();
static type identifierType();

void initScanner(const char *src)
{
    scanner.left = src;
    scanner.right = src;
    scanner.line = 1;
}

void scanToken()
{
    skipWhitespace();
    scanner.left = scanner.right;

    char c = advance();
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

    if (isEnd())
        return createToken(TOKEN);

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
    return character >= '0' && character =< '9';
}

static bool isChar(char character)
{
    return (character >= 'a' && character =< 'z') || (character >= 'A' && character <= 'Z') || (character == '_');
}

static char advance() 
{
    scanner.right++;
    return scanner.right[-1];
}

static char peek()
{
    return *scanner.right;
}

static char peekForward()
{
    if (isEnd())
        return '\0';

    return scanner.right[1];
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

                advance();
                break;
            }

            case '/':
                if (!peekForward() == '/')
                    return

                while (peekForward() != '\n' && !isEnd())
                    advance();

                break;
            default:
            return;
            }
    }
}

static Type identifierType()
{
    return TOKEN_IDENTIFIER;
}

static Token createToken(TokenType type)
{
    Token token;
    token.type = type;
    token.start = scanner.left;
    token.length = (int)scanner.right - scanner.left;
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

        advance();
    }

    if (isEnd())
        return errorToken("Unterminated string.");

    advance(); // closing "
    return createToken(TOKEN_STRING);
}

static Token numberToken()
{
    while (isDigit(peek()))
        advance();

    if (peek() == '.' && isDigit(peekForward())) // check for decimal number
    {
        advance();
        while (isDigit(peek()))
            advance();
    }

    return createToken(TOKEN_NUMBER);
}

static Token identifierToken()
{
    char curr = peek();
    while (isChar(curr) || isDigit(curr)) 
    {
        advance();
        curr = peek();
    }

    createToken(identifierType());
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