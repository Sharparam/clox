#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "io.h"
#include "options.h"
#include "config.h"
#include "chunk.h"
#include "debug.h"
#include "vm.h"
#include "errors.h"

static void print_version(const char * const name) {
    printf(
        "%s version %d.%d.%d\nC standard: %ld\n",
        name,
        CLOX_VERSION_MAJOR,
        CLOX_VERSION_MINOR,
        CLOX_VERSION_PATCH,
        __STDC_VERSION__);
}

static void repl() {
    for (;;) {
        printf("> ");
        char *line = clox_read_line();
        clox_vm_interpret(line);
        free(line);
    }
}

static void run_file(const char * const path) {
    char *contents = clox_read_file(path);
    CloxInterpretResult result = clox_vm_interpret(contents);
    free(contents);

    if (result == INTERPRET_COMPILE_ERROR) {
        exit(CLOX_EXIT_COMPILE_ERROR);
    }

    if (result == INTERPRET_RUNTIME_ERROR) {
        exit(CLOX_EXIT_RUNTIME_ERROR);
    }
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

    if (options.index == argc) {
        repl();
    } else if (options.index == argc - 1) {
        char *scriptPath = argv[options.index];
        run_file(scriptPath);
    } else {
        fprintf(stderr, "Usage: %s [path]\n", progname);
        clox_vm_free();
        exit(CLOX_EXIT_USAGE_ERROR);
    }

    clox_vm_free();

    return 0;
}
