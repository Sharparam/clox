#pragma once

#include <stdint.h>

#include "clox_chunk.h"
#include "clox_value.h"

#define CLOX_VM_STACK_MAX 256

typedef enum CloxInterpretResult {
    INTERPRET_OK,
    INTERPRET_COMPILE_ERROR,
    INTERPRET_RUNTIME_ERROR
} CloxInterpretResult;

typedef struct CloxVM CloxVM;
struct CloxVM {
    CloxChunk *chunk;
    uint8_t *ip;
    CloxValue stack[CLOX_VM_STACK_MAX];
    CloxValue *stack_top;
};

void clox_vm_init();
CloxInterpretResult clox_vm_interpret(const char * const source);
void clox_vm_stack_push(CloxValue value);
CloxValue clox_vm_stack_pop();
void clox_vm_free();
