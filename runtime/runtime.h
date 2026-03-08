#ifndef VMP_RUNTIME_H
#define VMP_RUNTIME_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void decrypt_data(char* data, size_t len, char key);
void clear_data(char* data, size_t len);
void vm_interpreter(const unsigned char* bytecode, void* args[]);
void anti_debug_init(void);

#ifdef __cplusplus
}
#endif

#endif
