#include "chunk.h"
#include "common.h"
#include "debug.h"

int main(int argc, char *argv[]) {
    printf(
        "You left off here: "
        "https://craftinginterpreters.com/"
        "chunks-of-bytecode.html#line-information\n");
    Chunk chunk;
    initChunk(&chunk);
    int constant = addConstant(&chunk, 1.2);
    writeChunk(&chunk, OP_CONSTANT);
    writeChunk(&chunk, OP_RETURN);

    disassembleChunk(&chunk, "test chunk");
    freeChunk(&chunk);
}
