#ifndef clox_debug_h
#define clox_debug_h
#include "chunk.h"

void disassembleChunk(Chunk* chunk, const char* name);
int disassembleInstruction(Chunk* chunk, int offset);

// deleteme
char** getPrg();
uint8_t getPip();
// deleteme
#endif
