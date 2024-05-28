#include <stdlib.h>
#include "memory.h"

void *reallocate(void *pointer, size_t oldSize, size_t newSize)
{
    (void)oldSize;
    if (newSize == 0)
    {
        free(pointer);
        return NULL;
    }

    void *res = realloc(pointer, newSize);
    if (res == NULL)
        exit(1);
    return res;
}