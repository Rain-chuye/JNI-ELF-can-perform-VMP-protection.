#ifndef LINKER_H
#define LINKER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* vmp_load_library(const char* path);
void* vmp_get_symbol(void* handle, const char* symbol);

#ifdef __cplusplus
}
#endif

#endif // LINKER_H
