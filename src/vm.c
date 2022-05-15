#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "vm.h"
#include "compiler.h"
#include "config.h"
#include "value.h"
#include "debug.h"

static CloxVM vm;
static const void * const stack_end = vm.stack + sizeof(vm.stack) / sizeof(vm.stack[0]);

static CloxInterpretResult run() {

#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op) do { \
        CloxValue b = clox_vm_stack_pop(); \
        CloxValue a = clox_vm_stack_pop(); \
        clox_vm_stack_push(a op b); \
    } while (false)

    for (;;) {
#ifdef CLOX_DEBUG_TRACE_EXECUTION
        printf("   (S)    ");
        for (CloxValue *slot = vm.stack; slot < vm.stack_top; slot++) {
            printf("[ ");
            clox_value_print(*slot);
            printf(" ]");
        }
        printf("\n");
        clox_chunk_disassemble_instruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT:
                clox_vm_stack_push(READ_CONSTANT());
                break;

            case OP_CONSTANT_LONG: {
                uint8_t high = READ_BYTE() << 8;
                uint8_t low = READ_BYTE();
                size_t idx = high | low;
                CloxValue value = vm.chunk->constants.values[idx];
                clox_vm_stack_push(value);
                break;
            }

            case OP_ADD:
                BINARY_OP(+);
                break;

            case OP_SUBTRACT:
                BINARY_OP(-);
                break;

            case OP_MULTIPLY:
                BINARY_OP(*);
                break;

            case OP_DIVIDE:
                BINARY_OP(/);
                break;

            case OP_NEGATE:
                clox_vm_stack_push(-clox_vm_stack_pop());
                break;

            case OP_RETURN:
                clox_value_print(clox_vm_stack_pop());
                printf("\n");
                return INTERPRET_OK;
        }
    }

#undef BINARY_OP
#undef READ_CONSTANT
#undef READ_BYTE
}

static void reset_stack() {
    vm.stack_top = vm.stack;
}

void clox_vm_init() {
    reset_stack();
}

CloxInterpretResult clox_vm_interpret(const char * const source) {
    CloxChunk chunk;
    clox_chunk_init(&chunk);

    if (!clox_compiler_compile(source, &chunk)) {
        clox_chunk_free(&chunk);
        return INTERPRET_COMPILE_ERROR;
    }

    vm.chunk = &chunk;
    vm.ip = vm.chunk->code;

    CloxInterpretResult result = run();

    clox_chunk_free(&chunk);
    return result;
}

void clox_vm_stack_push(CloxValue value) {
    if (vm.stack_top == stack_end) {
        fprintf(stderr, " /!\\ STACK OVERFLOW /!\\\n");
        abort();
    }

    *vm.stack_top++ = value;
}

CloxValue clox_vm_stack_pop() {
    return *(--vm.stack_top);
}

void clox_vm_free() {

}
