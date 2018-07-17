#include <stdio.h>

#include "clox_compiler.h"
#include "clox_scanner.h"

void clox_compile(const char * const source) {
    clox_scanner_init(source);

    int line = -1;

    for (;;) {
        CloxToken token = clox_scanner_scan_token();

        if (token.line != line) {
            printf("%4d ", token.line);
            line = token.line;
        } else {
            printf("   | ");
        }

        printf("%2d '%.*s'\n", token.type, token.length, token.start);

        if (token.type == TOKEN_EOF)
            break;
    }
}
