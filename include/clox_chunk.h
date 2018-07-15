#pragma once

#include <stdint.h>

#include "clox_value.h"

typedef enum OpCode {
    OP_CONSTANT,
    OP_CONSTANT_LONG,
    OP_RETURN
} OpCode;

typedef struct CloxChunk CloxChunk;
struct CloxChunk {
    int count;
    int capacity;
    uint8_t *code;
    int *line_numbers;
    CloxValueArray constants;
};

void clox_chunk_init(CloxChunk * const chunk);
void clox_chunk_write(CloxChunk * const chunk, uint8_t byte, int line);
void clox_chunk_free(CloxChunk * const chunk);

void clox_chunk_write_constant(CloxChunk * const chunk, int index, int line);

int clox_chunk_add_constant(CloxChunk * const chunk, CloxValue value);
