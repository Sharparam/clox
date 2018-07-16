#pragma once

#include <stdint.h>

#include "clox_chunk.h"

typedef enum CloxInterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} CloxInterpretResult;

typedef struct CloxVM CloxVM;
struct CloxVM {
    CloxChunk *chunk;
    uint8_t *ip;
};

void clox_vm_init();
CloxInterpretResult clox_vm_interpret(CloxChunk * const chunk);
void clox_vm_free();
