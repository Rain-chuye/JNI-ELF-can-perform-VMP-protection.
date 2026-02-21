#include "vmp_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <unistd.h>

void vmp_security_check() {
    // Basic anti-debug: Check if we can ptrace ourselves
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        // Already being traced or failed
        // For a real app, you might want to exit(0)
        // exit(1);
    }
}

void vmp_decrypt(uint8_t* data, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}

void vmp_interpreter(uint8_t* bytecode, void** args) {
    VMContext ctx;
    ctx.ip = bytecode;
    ctx.sp = -1;
    ctx.vars = args;

    vmp_security_check();

    while (1) {
        uint8_t opcode = *ctx.ip++;
        switch (opcode) {
            case OP_HALT:
                return;
            case OP_PUSH_CONST: {
                int64_t val;
                memcpy(&val, ctx.ip, sizeof(int64_t));
                ctx.ip += sizeof(int64_t);
                ctx.stack[++ctx.sp] = val;
                break;
            }
            case OP_PUSH_VAR: {
                uint8_t idx = *ctx.ip++;
                ctx.stack[++ctx.sp] = (int64_t)ctx.vars[idx];
                break;
            }
            case OP_POP_VAR: {
                uint8_t idx = *ctx.ip++;
                ctx.vars[idx] = (void*)ctx.stack[ctx.sp--];
                break;
            }
            case OP_ADD:
                ctx.stack[ctx.sp - 1] = ctx.stack[ctx.sp - 1] + ctx.stack[ctx.sp];
                ctx.sp--;
                break;
            case OP_SUB:
                ctx.stack[ctx.sp - 1] = ctx.stack[ctx.sp - 1] - ctx.stack[ctx.sp];
                ctx.sp--;
                break;
            case OP_MUL:
                ctx.stack[ctx.sp - 1] = ctx.stack[ctx.sp - 1] * ctx.stack[ctx.sp];
                ctx.sp--;
                break;
            case OP_XOR:
                ctx.stack[ctx.sp - 1] = ctx.stack[ctx.sp - 1] ^ ctx.stack[ctx.sp];
                ctx.sp--;
                break;
            case OP_JMP: {
                int32_t offset;
                memcpy(&offset, ctx.ip, sizeof(int32_t));
                ctx.ip = bytecode + offset;
                break;
            }
            case OP_JZ: {
                int32_t offset;
                memcpy(&offset, ctx.ip, sizeof(int32_t));
                ctx.ip += sizeof(int32_t);
                if (ctx.stack[ctx.sp--] == 0) ctx.ip = bytecode + offset;
                break;
            }
            case OP_RET:
                return;
            case OP_PRINT: {
                // PoC opcode to print top of stack
                printf("[VMP] Stack Top: %ld\n", ctx.stack[ctx.sp--]);
                break;
            }
            case OP_CALL_NATIVE: {
                // Complex to implement in pure C without libffi
                // For MVP, we use specific hardcoded native calls or skip
                ctx.ip++; // skip for now
                break;
            }
            default:
                return;
        }
    }
}
