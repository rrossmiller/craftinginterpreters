#include "vm.h"

#include <stdio.h>

#include "common.h"
#include "debug.h"
#include "value.h"
VM vm;

void resetStack() {
    vm.stackTop = vm.stack;
}
void initVM() {
    resetStack();
}

void freeVM() {
}

// static makes the function private
static InterpretResult run() {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define BINARY_OP(op)     \
    do {                  \
        double b = pop(); \
        double a = pop(); \
        push(a op b);     \
    } while (false)
    // explaination of the do while here:
    // https://craftinginterpreters.com/a-virtual-machine.html#binary-operators

    for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
        // print the stack
        printf("          ");
        // printf("stack:    ");
        for (Value* slot = vm.stack; slot < vm.stackTop; slot++) {
            printf("[ ");
            printValue(*slot);
            printf(" ]");
        }
        printf("\n");
        // print the current instruction
        disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

        uint8_t instruction;
        switch (instruction = READ_BYTE()) {
            case OP_CONSTANT: {
                Value constant = READ_CONSTANT();
                push(constant);
                break;
            }
            case OP_ADD: {
                BINARY_OP(+);
                break;
            }
            case OP_SUBSTRACT: {
                BINARY_OP(-);
                break;
            }
            case OP_MULTIPLY: {
                BINARY_OP(*);
                break;
            }
            case OP_DIVIDE: {
                BINARY_OP(/);
                break;
            }
            case OP_NEGATE: {
                Value val = pop();
                push(-val);

                break;
            }
            case OP_RETURN: {
                Value rtn = pop();
                printValue(rtn);
                printf("\n");
                return INTERPRET_OK;
            }
        }
    }
#undef READ_BYTE
#undef READ_CONSTANT
#undef BINARY_OP
}

InterpretResult interpret(Chunk* chunk) {
    // store the chunk being executed
    vm.chunk = chunk;

    // ip starts at the first byte in the code
    vm.ip = vm.chunk->code;

    // run the instructions
    return run();
}

void push(Value value) {
    // deref so the actual value at the top of the stack (under the current
    // pointer) is "value"
    *vm.stackTop = value;

    // incr the stack pointer
    vm.stackTop++;
}

Value pop() {
    // get the previous value
    vm.stackTop--;
    return *vm.stackTop;
}
