#include <stdio.h>

#include "chunk.h"
#include "common.h"
#include "debug.h"
#include "vm.h"

void t(Chunk* chunk);
int main(int argc, char* argv[]) {
    initVM();
    Chunk chunk;
    initChunk(&chunk);

    t(&chunk);
    printf("\nProgram Start:\n");
    interpret(&chunk);

    freeVM();
    freeChunk(&chunk);

#ifdef DEBUG_TRACE_commands
    printf("\n\n");
    char** prg = getPrg();
    for (int i = 0; i < getPip(); i++) {
        printf("%s\n", prg[i]);
    }
    printf("\n\ndone");
#endif
}

void t(Chunk* chunk) {
    int constant = addConstant(chunk, 1);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    constant = addConstant(chunk, 2);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    // add the last two stack vals
    writeChunk(chunk, OP_ADD, 123);

    constant = addConstant(chunk, 3);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    // add the last two stack vals
    writeChunk(chunk, OP_ADD, 123);

    constant = addConstant(chunk, 3.0);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    // divide the last two stack vals
    writeChunk(chunk, OP_DIVIDE, 123);

    writeChunk(chunk, OP_NEGATE, 123);
    writeChunk(chunk, OP_RETURN, 123);
}
void b(Chunk* chunk) {
    // add 1.2 to the stack
    int constant = addConstant(chunk, 1.2);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    // add 3.4 to the stack
    constant = addConstant(chunk, 3.4);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    // add the last two stack vals
    writeChunk(chunk, OP_ADD, 123);

    // add 5.6 to the stack
    constant = addConstant(chunk, 5.6);
    writeChunk(chunk, OP_CONSTANT, 123);
    writeChunk(chunk, constant, 123);

    // divide the last two stack vals
    writeChunk(chunk, OP_DIVIDE, 123);

    writeChunk(chunk, OP_NEGATE, 123);
    writeChunk(chunk, OP_RETURN, 123);
}
