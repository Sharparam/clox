#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "clox_vm.h"
#include "clox_compiler.h"
#include "clox_config.h"
#include "clox_value.h"
#include "clox_debug.h"

static CloxVM vm;
static const void * const stack_end = vm.stack + sizeof(vm.stack) / sizeof(vm.stack[0]);

static CloxInterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG() (vm.chunk->constants.values[(READ_BYTE() << 8) | READ_BYTE()])
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

            case OP_CONSTANT_LONG:
                clox_vm_stack_push(READ_CONSTANT_LONG());
                break;

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
#undef READ_CONSTANT_LONG
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
    clox_compile(source);
    run();
    return INTERPRET_OK;
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
