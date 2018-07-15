#include <stdint.h>
#include <stdio.h>

#include "clox_debug.h"
#include "clox_value.h"

static const char * const OPCODE_NAMES[] = {
    "OP_CONSTANT",
    "OP_RETURN"
};

static int instruction_simple(uint8_t instruction, int offset) {
    printf("%-16s\n", OPCODE_NAMES[instruction]);
    return offset + 1;
}

static int instruction_constant(uint8_t instruction, const CloxChunk * const chunk, int offset) {
    uint8_t constantIndex = chunk->code[offset + 1];
    printf("%-16s %4d '", OPCODE_NAMES[instruction], constantIndex);
    clox_value_print(chunk->constants.values[constantIndex]);
    printf("'\n");
    return offset + 2;
}

void clox_chunk_disassemble(const CloxChunk * const chunk, const char * const name) {
    printf("== %s ==\n", name);

    for (int i = 0; i < chunk->count;) {
        i = clox_chunk_disassemble_instruction(chunk, i);
    }
}

int clox_chunk_disassemble_instruction(const CloxChunk * const chunk, int offset) {
    printf("%04d ", offset);

    if (offset > 0 && chunk->line_numbers[offset] == chunk->line_numbers[offset - 1]) {
        printf("   | ");
    } else {
        printf("%4d ", chunk->line_numbers[offset]);
    }

    uint8_t instruction = chunk->code[offset];

    switch (instruction) {
        case OP_CONSTANT:
            return instruction_constant(instruction, chunk, offset);

        case OP_RETURN:
            return instruction_simple(instruction, offset);

        default:
            printf("Unknown opcode %d\n", instruction);
            return offset + 1;
    }
}
