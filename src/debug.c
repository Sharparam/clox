#include <stdint.h>
#include <stdio.h>

#include "debug.h"
#include "value.h"

static int instruction_simple(const char * const name, int offset) {
    printf("%-16s\n", name);
    return offset + 1;
}

static int instruction_constant(const char * const name, const CloxChunk * const chunk, int offset) {
    uint8_t constantIndex = chunk->code[offset + 1];
    printf("%-16s 0x%02x '", name, constantIndex);
    clox_value_print(chunk->constants.values[constantIndex]);
    printf("'\n");
    return offset + 2;
}

static int instruction_constant_long(const char * const name, const CloxChunk * const chunk, int offset) {
    uint16_t constantIndex = (chunk->code[offset + 1] << 8) | chunk->code[offset + 2];
    printf("%-16s 0x%04x '", name, constantIndex);
    clox_value_print(chunk->constants.values[constantIndex]);
    printf("'\n");
    return offset + 3;
}

void clox_chunk_disassemble(const CloxChunk * const chunk, const char * const name) {
    printf("== %s ==\n", name);

    for (int i = 0; i < chunk->count;) {
        i = clox_chunk_disassemble_instruction(chunk, i);
    }
}

int clox_chunk_disassemble_instruction(const CloxChunk * const chunk, int offset) {
    printf("0x%04x ", offset);

    if (offset > 0 && chunk->line_numbers[offset] == chunk->line_numbers[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->line_numbers[offset]);
    }

    uint8_t instruction = chunk->code[offset];

#define SIMPLE_CASE(op) case op: return instruction_simple(#op, offset)
#define CHUNK_CASE(op, func) case op: return func(#op, chunk, offset)

    switch (instruction) {
        CHUNK_CASE(OP_CONSTANT, instruction_constant);
        CHUNK_CASE(OP_CONSTANT_LONG, instruction_constant_long);
        SIMPLE_CASE(OP_ADD);
        SIMPLE_CASE(OP_SUBTRACT);
        SIMPLE_CASE(OP_MULTIPLY);
        SIMPLE_CASE(OP_DIVIDE);
        SIMPLE_CASE(OP_NEGATE);
        SIMPLE_CASE(OP_RETURN);

        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }

#undef CHUNK_CASE
#undef SIMPLE_CASE
}
