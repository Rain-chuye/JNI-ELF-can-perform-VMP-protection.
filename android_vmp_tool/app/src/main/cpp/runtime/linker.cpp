#include "linker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <dlfcn.h>
#include <android/log.h>

#define LOG_TAG "VMP_Linker"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if defined(__arm__)
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Sym  Elf32_Sym
#define Elf_Rel  Elf32_Rel
#define Elf_Rela Elf32_Rela
#define Elf_Dyn  Elf32_Dyn
#define ELF_R_TYPE(x) ELF32_R_TYPE(x)
#define ELF_R_SYM(x)  ELF32_R_SYM(x)
#else
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Sym  Elf64_Sym
#define Elf_Rel  Elf64_Rel
#define Elf_Rela Elf64_Rela
#define Elf_Dyn  Elf64_Dyn
#define ELF_R_TYPE(x) ELF64_R_TYPE(x)
#define ELF_R_SYM(x)  ELF64_R_SYM(x)
#endif

void* vmp_load_library(const char* path) {
    LOGI("Loading library: %s", path);
    int fd = open(path, O_RDONLY);
    if (fd < 0) return NULL;

    Elf_Ehdr ehdr;
    read(fd, &ehdr, sizeof(ehdr));

    // 1. Map segments
    size_t min_vaddr = (size_t)-1, max_vaddr = 0;
    lseek(fd, ehdr.e_phoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf_Phdr phdr;
        read(fd, &phdr, sizeof(phdr));
        if (phdr.p_type == PT_LOAD) {
            if (phdr.p_vaddr < min_vaddr) min_vaddr = phdr.p_vaddr;
            if (phdr.p_vaddr + phdr.p_memsz > max_vaddr) max_vaddr = phdr.p_vaddr + phdr.p_memsz;
        }
    }

    void* load_addr = mmap(NULL, max_vaddr - min_vaddr, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    uintptr_t base = (uintptr_t)load_addr - min_vaddr;

    lseek(fd, ehdr.e_phoff, SEEK_SET);
    for (int i = 0; i < ehdr.e_phnum; i++) {
        Elf_Phdr phdr;
        read(fd, &phdr, sizeof(phdr));
        if (phdr.p_type == PT_LOAD) {
            lseek(fd, phdr.p_offset, SEEK_SET);
            read(fd, (void*)(base + phdr.p_vaddr), phdr.p_filesz);
        }
    }
    close(fd);

    // 2. Relocations (Simplified logic for PoC)
    // Find dynamic section
    Elf_Dyn* dynamic = NULL;
    lseek(fd, ehdr.e_phoff, SEEK_SET); // Re-open or use cached
    // ... logic to find PT_DYNAMIC and parse it ...

    LOGI("Library loaded at %p", load_addr);
    return load_addr;
}
