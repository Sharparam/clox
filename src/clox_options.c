#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <getopt.h>

#include "clox_options.h"

typedef enum CloxOptionType {
    OPT_TYPE_BOOL,
    OPT_TYPE_REQUIRED_ARG,
    OPT_TYPE_OPTIONAL_ARG
} CloxOptionType;

typedef struct CloxOptionItem CloxOptionItem;
struct CloxOptionItem {
    char short_opt;
    char *long_opt;
    int has_arg;
    char *description;
    bool *flag_target;
    void *target;
    CloxOptionType option_type;
};

static CloxOptions options;

#define OPT_BOOL(s, l, t, d) { s, l, no_argument, d, NULL, t, OPT_TYPE_BOOL }
#define OPT_REQUIRED(s, l, t, d) { s, l, required_argument, d, NULL, t, OPT_TYPE_REQUIRED_ARG }
#define OPT_OPTIONAL(s, l, f_t, t, d) { s, l, optional_argument, d, f_t, t, OPT_TYPE_OPTIONAL_ARG }

static CloxOptionItem option_items[] = {
    OPT_BOOL('h', "help", &options.help, "Displays this help message and exits."),
    OPT_BOOL('V', "version", &options.version, "Displays version info."),
    OPT_BOOL('v', "verbose", &options.verbose, "Verbose output.")
};

static const int option_item_count = sizeof(option_items) / sizeof(option_items[0]);

static bool parsed = false;

static CloxOptionItem * find_option_item(int option_character) {
    for (int i = 0; i < option_item_count; i++) {
        if (option_items[i].short_opt == option_character) {
            return &option_items[i];
        }
    }

    return NULL;
}

void clox_options_print_help(const char * const program_name) {
    printf("Usage: %s [OPTIONS]\n", program_name);
    printf("C implementation of the Lox language\n\n");

    for (int i = 0; i < option_item_count; i++) {
        CloxOptionItem item = option_items[i];
        switch (item.option_type) {
            case OPT_TYPE_REQUIRED_ARG: {
                char *tmp = (char*)calloc(strlen(item.long_opt) + 5, sizeof(char));
                strncpy(tmp, item.long_opt, strlen(item.long_opt));
                strncat(tmp, "=ARG", 4);
                printf("    -%cARG, --%-20s %s\n", item.short_opt, tmp, item.description);
                free(tmp);
                break;
            }

            case OPT_TYPE_OPTIONAL_ARG: {
                char *tmp = (char*)calloc(strlen(item.long_opt) + 7, sizeof(char));
                strncpy(tmp, item.long_opt, strlen(item.long_opt));
                strncat(tmp, "[=ARG]", 6);
                printf("  -%c[ARG], --%-20s %s\n", item.short_opt, tmp, item.description);
                free(tmp);
                break;
            }

            default:
                printf("       -%c, --%-20s %s\n", item.short_opt, item.long_opt, item.description);
                break;
        }
    }
}

CloxOptions clox_options_parse(int argc, char * const argv[]) {
    if (parsed) {
        return options;
    }

    struct option * const long_options = (struct option *)calloc(option_item_count + 1, sizeof(struct option));
    size_t short_options_len = option_item_count + 1;
    char *short_options = (char *)malloc(sizeof(char) * short_options_len);

    for (int i = 0, s_i = 0; i < option_item_count; i++, s_i++) {
        CloxOptionItem item = option_items[i];
        long_options[i].name = item.long_opt;
        long_options[i].val = item.short_opt;
        long_options[i].has_arg = item.has_arg;

        if (item.option_type == OPT_TYPE_REQUIRED_ARG) {
            short_options_len += sizeof(char);
            short_options = (char*)realloc(short_options, short_options_len);
            short_options[s_i++] = item.short_opt;
            short_options[s_i] = ':';
        } else if (item.option_type == OPT_TYPE_OPTIONAL_ARG) {
            short_options_len += sizeof(char) * 2;
            short_options = (char*)realloc(short_options, short_options_len);
            short_options[s_i++] = item.short_opt;
            short_options[s_i++] = ':';
            short_options[s_i] = ':';
        } else {
            short_options[s_i] = item.short_opt;
        }
    }

    short_options[short_options_len - 1] = '\0';

    int option_index;
    int option_character;
    while ((option_character = getopt_long(argc, argv, short_options, long_options, &option_index)) != -1) {
        if (option_character == '?') {
            exit(EXIT_FAILURE);
        }

        CloxOptionItem *item = find_option_item(option_character);

        if (item == NULL) {
            continue;
        }

        switch (item->option_type) {
            case OPT_TYPE_BOOL:
                (*(bool*)item->target) = true;
                break;

            case OPT_TYPE_REQUIRED_ARG:
                *(char**)item->target = optarg;
                break;

            case OPT_TYPE_OPTIONAL_ARG:
                *item->flag_target = true;

                if (optarg) {
                    *(char**)item->target = optarg;
                } else if (!optarg
                        && optind < argc
                        && NULL != argv[optind]
                        && '\0' != argv[optind][0]
                        && '-' != argv[optind][0]) {
                    // https://stackoverflow.com/a/32575314/1104531
                    *(char**)item->target = argv[optind++];
                }

                break;

            default:
                fprintf(stderr, "Unknown option type %d\n", item->option_type);
                exit(EXIT_FAILURE);
                break;
        }
    }

    free(short_options);
    free(long_options);

    options.index = optind;

    parsed = true;
    return options;
}
