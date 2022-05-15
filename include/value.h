#pragma once

typedef double CloxValue;

typedef struct CloxValueArray CloxValueArray;
struct CloxValueArray {
    int capacity;
    int count;
    CloxValue *values;
};

void clox_value_print(CloxValue value);

void clox_valuearray_init(CloxValueArray * const array);

void clox_valuearray_write(CloxValueArray * const array, CloxValue value);

void clox_valuearray_free(CloxValueArray * const array);
