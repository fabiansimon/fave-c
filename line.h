#ifndef LINE_H
#define LINE_H

#include "common.h"

typedef struct 
{
    int capacity;
    int count;
    int *lines;
} LineArray;

void initLineArray(LineArray *array);
void writeLineArray(LineArray *array, int line);
void freeLineArray(LineArray *array);
int getLine(LineArray *array, int *offset);

#endif