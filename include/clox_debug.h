#pragma once

#include "clox_chunk.h"

void clox_chunk_disassemble(const CloxChunk * const chunk, const char * const name);
int clox_chunk_disassemble_instruction(const CloxChunk * const chunk, int offset);
