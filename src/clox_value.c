#include <stdlib.h>
#include <stdio.h>

#include "clox_memory.h"
#include "clox_value.h"

void clox_value_print(CloxValue value) {
    printf("%g", value);
}

void clox_valuearray_init(CloxValueArray * const array) {
    array->capacity = 0;
    array->count = 0;
    array->values = NULL;
}

void clox_valuearray_write(CloxValueArray * const array, CloxValue value) {
    if (array->capacity < array->count + 1) {
        int oldCapacity = array->capacity;
        array->capacity = CLOX_GROW_CAPACITY(array->capacity);
        array->values = CLOX_GROW_ARRAY(array->values, CloxValue, oldCapacity, array->capacity);
    }

    array->values[array->count++] = value;
}

void clox_valuearray_free(CloxValueArray * const array) {
    CLOX_FREE_ARRAY(CloxValue, array->values, array->capacity);
    clox_valuearray_init(array);
}
