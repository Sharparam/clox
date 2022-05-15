#include <stdlib.h>

#include "memory.h"

void *reallocate(void *previous, size_t oldSize, size_t newSize) {
    if (newSize == 0) {
        free(previous);
        return NULL;
    }

    if (oldSize == 0) {
        // New allocation
    }

    return realloc(previous, newSize);
}
