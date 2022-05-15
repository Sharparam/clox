#pragma once

#include <stdbool.h>

typedef struct CloxOptions CloxOptions;
struct CloxOptions {
    bool help;
    bool version;
    bool verbose;
    int index;
};

void clox_options_print_help(const char * const program_name);
CloxOptions clox_options_parse(int argc, char * const argv[]);
