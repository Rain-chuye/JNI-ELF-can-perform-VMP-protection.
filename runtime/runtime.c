#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>

void decrypt_data(char* data, size_t len, char key) {
    if (!data) return;
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key;
    }
}

void clear_data(char* data, size_t len) {
    if (!data) return;
    memset(data, 0, len);
}

void anti_debug_init(void) {
#ifdef __linux__
    // Anti-ptrace: if we can't be traced, someone else is already tracing us
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        if (errno != EPERM) {
            // Real debugger detected or something wrong
            exit(1);
        }
    } else {
        // We are successfully tracing ourselves, now detach
        ptrace(PTRACE_DETACH, 0, 1, 0);
    }

    // Check TracerPid in /proc/self/status
    FILE* fp = fopen("/proc/self/status", "r");
    if (fp) {
        char line[256];
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "TracerPid:", 10) == 0) {
                int pid = atoi(&line[10]);
                if (pid != 0) {
                    // Tracer detected!
                    exit(1);
                }
                break;
            }
        }
        fclose(fp);
    }
#endif
}

// Stack-based VM Implementation
#define MAX_STACK 1024
#define OP_PUSH   0x10
#define OP_POP    0x11
#define OP_ADD    0x20
#define OP_SUB    0x21
#define OP_MUL    0x22
#define OP_LOAD   0x30
#define OP_STORE  0x31
#define OP_CALL   0x40
#define OP_JMP    0x50
#define OP_JZ     0x51
#define OP_EXIT   0xFF

void vm_interpreter(const unsigned char* bytecode, void* args[]) {
    int64_t stack[MAX_STACK];
    int sp = -1;
    int pc = 0;
    int64_t registers[16] = {0};

    while (1) {
        unsigned char opcode = bytecode[pc++];
        switch (opcode) {
            case OP_PUSH: {
                int64_t val;
                memcpy(&val, &bytecode[pc], 8);
                pc += 8;
                stack[++sp] = val;
                break;
            }
            case OP_POP:
                sp--;
                break;
            case OP_ADD: {
                int64_t b = stack[sp--];
                int64_t a = stack[sp--];
                stack[++sp] = a + b;
                break;
            }
            case OP_SUB: {
                int64_t b = stack[sp--];
                int64_t a = stack[sp--];
                stack[++sp] = a - b;
                break;
            }
            case OP_MUL: {
                int64_t b = stack[sp--];
                int64_t a = stack[sp--];
                stack[++sp] = a * b;
                break;
            }
            case OP_LOAD: {
                int idx = bytecode[pc++];
                stack[++sp] = (int64_t)args[idx];
                break;
            }
            case OP_EXIT:
                return;
            default:
                return;
        }
    }
}

__attribute__((constructor))
void vmp_init() {
    anti_debug_init();
}
