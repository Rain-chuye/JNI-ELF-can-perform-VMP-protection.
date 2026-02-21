#ifndef RUNTIME_H
#define RUNTIME_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void decrypt_data(char* data, size_t len, char key);
void vm_interpreter(const unsigned char* bytecode, void* args[]);

#ifdef __cplusplus
}
#endif

#endif // RUNTIME_H
