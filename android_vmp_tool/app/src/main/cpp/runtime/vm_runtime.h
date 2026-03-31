#ifndef VM_RUNTIME_H
#define VM_RUNTIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint8_t opcode;
    uint32_t arg;
} VMPInstruction;

void vmp_execute(const uint8_t* bytecode, void* context);

#ifdef __cplusplus
}
#endif

#endif // VM_RUNTIME_H
