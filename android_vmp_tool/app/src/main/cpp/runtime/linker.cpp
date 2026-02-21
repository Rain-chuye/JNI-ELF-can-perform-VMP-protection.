#include "linker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <elf.h>
#include <dlfcn.h>
#include <android/log.h>

#define LOG_TAG "VMP_Linker"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#ifdef __arm__
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Sym  Elf32_Sym
#define Elf_Dyn  Elf32_Dyn
#define Elf_Rel  Elf32_Rel
#define ELF_R_SYM(i) ELF32_R_SYM(i)
#define ELF_R_TYPE(i) ELF32_R_TYPE(i)
#else
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Sym  Elf64_Sym
#define Elf_Dyn  Elf64_Dyn
#define Elf_Rela Elf64_Rela
#define ELF_R_SYM(i) ELF64_R_SYM(i)
#define ELF_R_TYPE(i) ELF64_R_TYPE(i)
#endif

void* vmp_load_library_from_mem(void* buffer, size_t size) {
    Elf_Ehdr* ehdr = (Elf_Ehdr*)buffer;
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) return NULL;

    // 1. Map segments
    Elf_Phdr* phdr = (Elf_Phdr*)((uint8_t*)buffer + ehdr->e_phoff);
    uintptr_t min_vaddr = (uintptr_t)-1, max_vaddr = 0;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            if (phdr[i].p_vaddr < min_vaddr) min_vaddr = phdr[i].p_vaddr;
            if (phdr[i].p_vaddr + phdr[i].p_memsz > max_vaddr) max_vaddr = phdr[i].p_vaddr + phdr[i].p_memsz;
        }
    }

    void* load_addr = mmap(NULL, max_vaddr - min_vaddr, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    uintptr_t base = (uintptr_t)load_addr - min_vaddr;

    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr[i].p_type == PT_LOAD) {
            memcpy((void*)(base + phdr[i].p_vaddr), (uint8_t*)buffer + phdr[i].p_offset, phdr[i].p_filesz);
        }
    }

    // 2. Handle Relocations (Simplified for PoC, real one needs symbol lookup via dlsym)
    // Most JNI libs use R_RELATIVE which we can handle easily.

    LOGI("Library loaded in memory at %p", load_addr);
    return load_addr;
}
