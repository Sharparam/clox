#include <stdlib.h>

#include "clox_chunk.h"
#include "clox_memory.h"

void clox_chunk_init(CloxChunk * const chunk) {
    chunk->capacity = 0;
    chunk->count = 0;
    chunk->code = NULL;
    chunk->line_numbers = NULL;
    clox_valuearray_init(&chunk->constants);
}

void clox_chunk_write(CloxChunk * const chunk, uint8_t byte, int line) {
    if (chunk->capacity < chunk->count + 1) {
        int oldCapacity = chunk->capacity;
        chunk->capacity = CLOX_GROW_CAPACITY(chunk->capacity);
        chunk->code = CLOX_GROW_ARRAY(
            chunk->code,
            uint8_t,
            oldCapacity,
            chunk->capacity);
        chunk->line_numbers = CLOX_GROW_ARRAY(
            chunk->line_numbers,
            int,
            oldCapacity,
            chunk->capacity);
    }

    chunk->code[chunk->count] = byte;
    chunk->line_numbers[chunk->count++] = line;
}

void clox_chunk_free(CloxChunk * const chunk) {
    clox_valuearray_free(&chunk->constants);
    CLOX_FREE_ARRAY(uint8_t, chunk->code, chunk->capacity);
    CLOX_FREE_ARRAY(int, chunk->line_numbers, chunk->capacity);
    clox_chunk_init(chunk);
}

void clox_chunk_write_constant(CloxChunk * const chunk, int index, int line) {
    uint8_t opcode = index > UINT8_MAX ? OP_CONSTANT_LONG : OP_CONSTANT;
    clox_chunk_write(chunk, opcode, line);

    if (opcode == OP_CONSTANT_LONG) {
        clox_chunk_write(chunk, index >> 8, line);
    }

    clox_chunk_write(chunk, index & UINT8_MAX, line);
}

int clox_chunk_add_constant(CloxChunk * const chunk, CloxValue value) {
    clox_valuearray_write(&chunk->constants, value);
    return chunk->constants.count - 1;
}
