#include <stdio.h>
#include <stdlib.h>

#include "clox_io.h"
#include "clox_errors.h"

char * clox_read_line() {
    size_t buffer_size = 1024;
    char *buffer = (char*)malloc(buffer_size * sizeof(char));

    if (buffer == NULL) {
        fprintf(stderr, "OUT OF MEMORY in clox_read_line (malloc).\n");
        exit(CLOX_EXIT_OOM_ERROR);
    }

    size_t num_read = 0;
    char next;

    do {
        next = fgetc(stdin);

        if (num_read >= buffer_size) {
            char *old_buffer = buffer;
            buffer_size *= 2;
            buffer = (char*)realloc(buffer, buffer_size);

            if (buffer == NULL) {
                free(old_buffer);
                fprintf(stderr, "OUT OF MEMORY in clox_read_line\n");
                exit(CLOX_EXIT_OOM_ERROR);
            }
        }

        buffer[num_read++] = next;
    } while (next != EOF && next != '\0' && next != '\n');

    buffer[num_read - 1] = '\0';
    return buffer;
}

char * clox_read_file(const char * const path) {
    FILE *file = fopen(path, "rb");

    if (file == NULL) {
        fprintf(stderr, "Failed to open \"%s\" for reading.\n", path);
        exit(CLOX_EXIT_FILE_ERROR);
    }

    fseek(file, 0L, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    char *buffer = (char*)malloc(fileSize + 1);

    if (buffer == NULL) {
        fprintf(stderr, "Failed to allocate memory for reading \"%s\".\n", path);
        exit(CLOX_EXIT_OOM_ERROR);
    }

    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

    if (bytesRead < fileSize) {
        fprintf(stderr, "Failed to read all bytes from \"%s\".\n", path);
        exit(CLOX_EXIT_FILE_ERROR);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}
