#include <stdio.h>

#include "clox_config.h"
#include "clox_chunk.h"
#include "clox_debug.h"

int main(int argc, const char *argv[]) {
    fprintf(
        stdout,
        "%s version %d.%d.%d\n",
        argv[0],
        clox_VERSION_MAJOR,
        clox_VERSION_MINOR,
        clox_VERSION_PATCH);

    printf("C standard: %ld\n", __STDC_VERSION__);

    CloxChunk chunk;

    clox_chunk_init(&chunk);

    int constantIndex = clox_chunk_add_constant(&chunk, 1.2);
    clox_chunk_write(&chunk, OP_CONSTANT, 1);
    clox_chunk_write(&chunk, constantIndex, 1);

    clox_chunk_write(&chunk, OP_RETURN, 1);

    clox_chunk_disassemble(&chunk, "Test chunk");

    clox_chunk_free(&chunk);

    return 0;
}
