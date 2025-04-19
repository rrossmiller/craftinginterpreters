#ifndef clox_vm_h
#define clox_vm_h
#include <stdlib.h>

#include "chunk.h"
#include "value.h"

#define STACK_MAX 256

typedef struct {
    Chunk* chunk;
    uint8_t* ip;  // instruction/byte pointer to where the vm is in the
                  // instruction set
    Value stack[STACK_MAX];
    Value* stackTop;
} VM;

typedef enum {
    INTERPRET_OK,
    INTERPRET_COPMILE_ERROR,
    INTERPRET_RUNTIME_ERROR,

} InterpretResult;

void initVM();
void freeVM();
InterpretResult interpret(Chunk* chunk);
void push(Value value);
Value pop();

#endif
