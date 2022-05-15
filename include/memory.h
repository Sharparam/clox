#pragma once

#include <stddef.h>

#define CLOX_GROW_CAPACITY(capacity) \
    ((capacity) < 8 ? 8 : (capacity) * 2)

#define CLOX_GROW_ARRAY(previous, type, oldCount, count) \
    (type *)reallocate(previous, sizeof(type) * (oldCount), sizeof(type) * (count))

#define CLOX_FREE_ARRAY(type, pointer, oldCount) \
    (type *)reallocate(pointer, sizeof(type) * (oldCount), 0);

void *reallocate(void *previous, size_t oldSize, size_t newSize);
