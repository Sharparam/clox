#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "clox_options.h"

typedef struct CloxOptionItem CloxOptionItem;
struct CloxOptionItem {
    char short_opt;
    char *long_opt;
    int has_arg;
    char *description;
};

#define OPT_BOOL(s, l, d) { s, l, no_argument, d }

static CloxOptions options;

static CloxOptionItem option_items[] = {
    OPT_BOOL('h', "help", "Displays this help message and exits."),
    OPT_BOOL('V', "version", "Displays version info.")
};

static const int option_item_count = sizeof(option_items) / sizeof(option_items[0]);

static bool parsed = false;

void clox_options_print_help(const char * const program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("C implementation of the Lox language\n\n");

    for (int i = 0; i < option_item_count; i++) {
        CloxOptionItem item = option_items[i];
        printf("  -%c, --%-16s %s\n", item.short_opt, item.long_opt, item.description);
    }
}

CloxOptions clox_options_parse(int argc, char * const argv[]) {
    if (parsed) {
        return options;
    }

    struct option * const long_options = (struct option *)calloc(option_item_count + 1, sizeof(struct option));
    char * const short_options = (char *)malloc(sizeof(char) * (option_item_count + 1));

    for (int i = 0; i < option_item_count; i++) {
        CloxOptionItem item = option_items[i];
        long_options[i].name = item.long_opt;
        long_options[i].val = item.short_opt;
        long_options[i].has_arg = item.has_arg;
        short_options[i] = item.short_opt;
    }

    short_options[option_item_count] = '\0';

    int option_index;
    int option_character;
    while ((option_character = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        switch (option_character) {
            case 'h':
                options.help = true;
                break;

            case 'V':
                options.version = true;
                break;
        }
    }

    free(short_options);
    free(long_options);

    options.index = optind;

    parsed = true;
    return options;
}
