#ifndef LINKER_H
#define LINKER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* vmp_load_library_from_mem(void* buffer, size_t size);
void* vmp_get_symbol_from_lib(void* handle, const char* name);
void* vmp_find_symbol(void* handle, const char* name);

#ifdef __cplusplus
}
#endif

#endif
