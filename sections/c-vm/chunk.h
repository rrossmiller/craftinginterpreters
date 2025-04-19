#ifndef clox_chunk_h
#define clox_chunk_h

#include "common.h"
#include "value.h"

typedef enum {
    OP_CONSTANT,
    OP_NEGATE,
    OP_ADD,
    OP_SUBSTRACT,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_RETURN,
} OpCode;

// Chunk
typedef struct {
    int count;
    int capacity;
    uint8_t* code;
    int* lines;
    ValueArray constants;
} Chunk;

// TODO:
/*
   Our encoding of line information is hilariously wasteful of memory. Given
   that a series of instructions often correspond to the same source line, a
   natural solution is something akin to run-length encoding of the line
   numbers.

   Devise an encoding that compresses the line information for a series of
   instructions on the same line. Change writeChunk() to write this compressed
   form, and implement a getLine() function that, given the index of an
   instruction, determines the line where the instruction occurs.

   Hint: It’s not necessary for getLine() to be particularly efficient. Since
   it is called only when a runtime error occurs, it is well off the critical
   path where performance matters.
*/
void initChunk(Chunk* chunk);
void freeChunk(Chunk* chunk);
void writeChunk(Chunk* chunk, uint8_t byte, int line);
int addConstant(Chunk* chunk, Value value);

#endif
