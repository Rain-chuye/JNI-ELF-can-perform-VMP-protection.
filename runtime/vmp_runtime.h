#ifndef VMP_RUNTIME_H
#define VMP_RUNTIME_H

#include <stdint.h>
#include <stddef.h>

// VM Opcodes
typedef enum {
    OP_HALT = 0x00,
    OP_PUSH_CONST = 0x01,
    OP_PUSH_VAR = 0x02,
    OP_POP_VAR = 0x03,
    OP_ADD = 0x04,
    OP_SUB = 0x05,
    OP_MUL = 0x06,
    OP_DIV = 0x07,
    OP_AND = 0x08,
    OP_OR  = 0x09,
    OP_XOR = 0x0A,
    OP_NOT = 0x0B,
    OP_JMP = 0x0C,
    OP_JZ  = 0x0D,
    OP_JNZ = 0x0E,
    OP_CALL_NATIVE = 0x0F,
    OP_RET = 0x10,
    OP_LOAD = 0x11,
    OP_STORE = 0x12,
    OP_PRINT = 0x13,
} VMOpcode;

// VM Context
typedef struct {
    uint8_t* ip;      // Instruction Pointer
    int64_t stack[256];
    int sp;           // Stack Pointer
    void** vars;      // Variables/Arguments
} VMContext;

// Anti-debug / Anti-tamper
void vmp_security_check();

// Interpreter
void vmp_interpreter(uint8_t* bytecode, void** args);

// String decryption
void vmp_decrypt(uint8_t* data, size_t len, uint8_t key);

#endif
