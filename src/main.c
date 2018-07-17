#include <stdio.h>

#include "clox_options.h"
#include "clox_config.h"
#include "clox_chunk.h"
#include "clox_debug.h"
#include "clox_vm.h"

static void print_version(const char * const name) {
    printf(
        "%s version %d.%d.%d\nC standard: %ld\n",
        name,
        CLOX_VERSION_MAJOR,
        CLOX_VERSION_MINOR,
        CLOX_VERSION_PATCH,
        __STDC_VERSION__);
}

int main(int argc, char *argv[]) {
    const char * const progname = argv[0];

    CloxOptions options = clox_options_parse(argc, argv);

    if (options.help) {
        clox_options_print_help(progname);
        return 0;
    }

    if (options.version) {
        print_version(progname);
    }

    clox_vm_init();

    CloxChunk chunk;

    clox_chunk_init(&chunk);

    int constantIndex = clox_chunk_add_constant(&chunk, 1.2);
    clox_chunk_write_constant(&chunk, constantIndex, 1);

    constantIndex = clox_chunk_add_constant(&chunk, 3.4);
    clox_chunk_write_constant(&chunk, constantIndex, 1);

    clox_chunk_write(&chunk, OP_ADD, 1);

    constantIndex = clox_chunk_add_constant(&chunk, 5.6);
    clox_chunk_write_constant(&chunk, constantIndex, 1);

    clox_chunk_write(&chunk, OP_DIVIDE, 1);
    clox_chunk_write(&chunk, OP_NEGATE, 1);
    clox_chunk_write(&chunk, OP_RETURN, 1);

    if (options.verbose) {
        clox_chunk_disassemble(&chunk, "Test chunk");
    }

    clox_vm_interpret(&chunk);

    clox_vm_free();
    clox_chunk_free(&chunk);

    return 0;
}
