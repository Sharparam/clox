#pragma once

#include <stdbool.h>

#include "clox_chunk.h"

bool clox_compiler_compile(const char * const source, CloxChunk *chunk);
