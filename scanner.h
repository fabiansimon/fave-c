#ifndef SCANNER_H
#define SCANNER_H

typedef struct
{
    const char *start;
    const char *curr;
    int line;
} Scanner;

void initScanner(const char *src);

#endif