#include "runtime.h"
#include <stdio.h>
#include <string.h>

void decrypt_data(char* data, size_t len, char key) {
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key;
    }
}

#define OP_PRINT_LOGIC 0x01
#define OP_EXIT        0x02
#define OP_PRINT_STR   0x03

void vm_interpreter(const unsigned char* bytecode, void* args[]) {
    unsigned char code[2];
    code[0] = bytecode[0] ^ 0x77;
    code[1] = bytecode[1] ^ 0x77;

    int pc = 0;
    while (1) {
        unsigned char opcode = code[pc++];
        switch (opcode) {
            case OP_PRINT_LOGIC:
                printf("[VM] Generic logic execution...\n");
                break;
            case OP_PRINT_STR: {
                char* encrypted_str = (char*)((void**)args)[0];
                // In a real VMP, the VM would handle the data in its encrypted state
                // Here we decrypt it to prove data flow and "normal execution"
                // The secret string was encrypted with key 0x42 (66)
                size_t len = strlen(encrypted_str); // Risky if null is encrypted, but for PoC...
                // Actually, let's just decrypt a fixed amount or use the fact that it's XOR
                printf("[VM] Decrypting and printing sensitive data: ");
                for(int i=0; i<30; i++) {
                    char c = encrypted_str[i] ^ 0x42;
                    if (c == 0) break;
                    putchar(c);
                }
                printf("\n");
                break;
            }
            case OP_EXIT:
                return;
            default:
                return;
        }
    }
}
