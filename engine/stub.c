#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <elf.h>
#include <link.h>

// This stub will be injected into the .so
// It finds the .rodata section and decrypts it in place.

void decrypt_section(void* addr, size_t len, char key) {
    // Make memory writable
    size_t pagesize = sysconf(_SC_PAGESIZE);
    void* page_start = (void*)((size_t)addr & ~(pagesize - 1));
    size_t page_len = ((size_t)addr + len - (size_t)page_start + pagesize - 1) & ~(pagesize - 1);

    if (mprotect(page_start, page_len, PROT_READ | PROT_WRITE | PROT_EXEC) < 0) {
        perror("mprotect");
        return;
    }

    char* data = (char*)addr;
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }

    // Restore permissions (optional, but safer)
    // mprotect(page_start, page_len, PROT_READ);
}

// We will pass the section offset and size via some global variables
// that we will patch after injection.
size_t target_offset = 0;
size_t target_len = 0;
char target_key = 0;

__attribute__((constructor))
void auto_decrypt() {
    if (target_len == 0) return;

    // Find our own load address
    // We can use dladdr or iterate over phdr
    // For simplicity, let's assume we can find our base address
    // In a real stub, we'd use something more robust.
}
