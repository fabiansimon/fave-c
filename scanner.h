#ifndef SCANNER_H
#define SCANNER_H

typedef struct
{
    const char *left;
    const char *right;
    int line;
} Scanner;

void initScanner(const char *src);
void scanToken();

#endif