#include "vm_runtime.h"
#include <sys/mman.h>
#include <unistd.h>
#include <android/log.h>

extern "C" void decrypt_section(void* addr, size_t size, uint8_t key) {
    uintptr_t start = (uintptr_t)addr & ~(getpagesize() - 1);
    uintptr_t end = ((uintptr_t)addr + size + getpagesize() - 1) & ~(getpagesize() - 1);

    mprotect((void*)start, end - start, PROT_READ | PROT_WRITE | PROT_EXEC);
    uint8_t* p = (uint8_t*)addr;
    for (size_t i = 0; i < size; i++) p[i] ^= key;
    mprotect((void*)start, end - start, PROT_READ | PROT_EXEC);
}
