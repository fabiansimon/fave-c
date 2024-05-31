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
static bool checkKeyword(int start, int length, const char *rest);
static TokenType identifierToken();

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

static TokenType identifierToken()
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

static TokenType identifyType(TokenType ) 
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
                    case 'a': return checkKeyword(2, 3, "lse", TOKEN_FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TOKEN_FOR);
                    case 'u': return checkKeyword(2, 2, "un", TOKEN_FUN);
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
                    case 'h': return checkKeyword(2, 2, "is", TOKEN_THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TOKEN_TRUE);
                    }
            }
        }
    } 
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
        advance();
        curr = peek();
    }

    createToken(identifyType());
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