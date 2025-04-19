#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "run.h"
#include "vm.h"

int main(int argc, char* argv[]) {
    initVM();

    if (argc == 1) {
        repl();
    } else if (argc == 2) {
        runFile(argv[1]);
    } else {
        fprintf(stderr, "Usage: clox [path]\n");
        exit(64);
    }

#ifdef DEBUG_TRACE_commands
    printf("\n\n");
    char** prg = getPrg();
    for (int i = 0; i < getPip(); i++) {
        printf("%s\n", prg[i]);
    }
    printf("\n\ndone");
#endif

    freeVM();
}
