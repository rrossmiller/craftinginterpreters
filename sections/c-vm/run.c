#include "run.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vm.h"

static char* readFile(const char* path);

void repl() {
    char line[1024];

    while (true) {
        printf("> ");
        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break;
        }
        // interpret(line);
    }
}

void runFile(const char* path) {
    char* source = readFile(path);

    InterpretResult result = interpret(source);
    free(source);

    if (result == INTERPRET_COPMILE_ERROR) {
        exit(65);
    }
    if (result == INTERPRET_RUNTIME_ERROR) {
        exit(70);
    }
}

static char* readFile(const char* path) {
    FILE* file = fopen(path, "rb");
    if (file == NULL) {
        fprintf(stderr, "Could not open file \"%s\"\n", path);
        exit(74);
    }

    // seek to the end to get the number of bytes in the file
    fseek(file, 0l, SEEK_END);
    size_t fileSize = ftell(file);
    rewind(file);

    // alloc a buffer to read the whole file
    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fprintf(stderr, "Not enough memory to read \"%s\"\n", path);
        exit(74);
    }
    size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);
    if (bytesRead < fileSize) {
        fprintf(stderr, "Could not read file \"%s\"\n", path);
        exit(74);
    }

    buffer[bytesRead] = '\0';

    fclose(file);
    return buffer;
}
