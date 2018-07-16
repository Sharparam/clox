#include <stdio.h>

#include "clox_vm.h"
#include "clox_value.h"

static CloxVM vm;

static CloxInterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG() (vm.chunk->constants.values[(READ_BYTE() << 8) | READ_BYTE()])

    for (;;) {
        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                CloxValue constant = READ_CONSTANT();
                clox_value_print(constant);
                printf("\n");
                break;
            }

            case OP_CONSTANT_LONG: {
                CloxValue constant = READ_CONSTANT_LONG();
                clox_value_print(constant);
                printf("\n");
                break;
            }

            case OP_RETURN:
                return INTERPRET_OK;
        }
    }

#undef READ_CONSTANT_LONG
#undef READ_CONSTANT
#undef READ_BYTE
}

void clox_vm_init() {

}

CloxInterpretResult clox_vm_interpret(CloxChunk * const chunk) {
    vm.chunk = chunk;
    vm.ip = vm.chunk->code;
    return run();
}

void clox_vm_free() {

}
