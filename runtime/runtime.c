#include "runtime.h"
#include <stdio.h>

void decrypt_data(char* data, size_t len, char key) {
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key;
    }
}

#define OP_PRINT_LOGIC 0x01
#define OP_EXIT        0x02

void vm_interpreter(const unsigned char* bytecode, void* args[]) {
    unsigned char code[2];
    code[0] = bytecode[0] ^ 0x77;
    code[1] = bytecode[1] ^ 0x77;

    int pc = 0;
    while (1) {
        unsigned char opcode = code[pc++];
        switch (opcode) {
            case OP_PRINT_LOGIC:
                printf("[VM] Virtualized execution of sensitive logic...\n");
                break;
            case OP_EXIT:
                return;
            default:
                return;
        }
    }
}
