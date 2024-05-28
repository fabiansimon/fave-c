#include "memory.h"
#include "line.h"

void initLineArray(LineArray *array) 
{
    array->count = 0;
    array->capacity = 0;
    array->lines = NULL;
}

void writeLineArray(LineArray *array, int line) 
{
    if (array->count + 1 >= array->capacity)
    {
        int oldCapacity = array->capacity;
        array->capacity = GROW_CAPACITY(oldCapacity);
        array->lines = GROW_ARRAY(int, array->lines, oldCapacity, array->capacity);
    }


    int count = array->count;

    /* Empty Line array */
    if (count == 0) 
    {
        array->lines[0] = 1;
        array->lines[1] = line;
        array->count = 2;
        return;
    }

    /* Matches the last line */
    if (array->lines[count - 1] == line)
    {
        array->lines[count - 2]++;
        return;
    }

    /* Record a new line */
    array->lines[count] = 1;
    array->lines[count + 1] = line;
    array->count += 2;
}

void freeLineArray(LineArray *array)
{
    FREE_ARRAY(int, array->lines, array->capacity);
    initLineArray(array);
}

int getLine(LineArray *array, int *offset)
{
    for (int i = 0, curr = 0; i < array->count; i+=2)
    {
        curr += array->lines[i];
        if (curr > *offset)
            return array->lines[i+1];
    }
    return -1;
}
