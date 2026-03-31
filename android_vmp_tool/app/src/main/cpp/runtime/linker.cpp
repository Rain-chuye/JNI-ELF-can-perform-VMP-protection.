#include "linker.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <elf.h>
#include <dlfcn.h>
#include <unistd.h>
#include <android/log.h>

#define LOG_TAG "VMP_Linker"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

#ifdef __arm__
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Sym  Elf32_Sym
#define Elf_Dyn  Elf32_Dyn
#define Elf_Rel  Elf32_Rel
#define ELF_R_SYM(i) ELF32_R_SYM(i)
#define ELF_R_TYPE(i) ELF32_R_TYPE(i)
#define REL_RELATIVE R_ARM_RELATIVE
#define REL_ABS      R_ARM_ABS32
#define REL_GLOB_DAT R_ARM_GLOB_DAT
#define REL_JUMP_SLOT R_ARM_JUMP_SLOT
#else
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Sym  Elf64_Sym
#define Elf_Dyn  Elf64_Dyn
#define Elf_Rela Elf64_Rela
#define ELF_R_SYM(i) ELF64_R_SYM(i)
#define ELF_R_TYPE(i) ELF64_R_TYPE(i)
#define REL_RELATIVE R_AARCH64_RELATIVE
#define REL_ABS      R_AARCH64_ABS64
#define REL_GLOB_DAT R_AARCH64_GLOB_DAT
#define REL_JUMP_SLOT R_AARCH64_JUMP_SLOT
#endif

typedef void (*init_func_t)();

void* vmp_load_library_from_mem(void* buffer, size_t size) {
    Elf_Ehdr* ehdr = (Elf_Ehdr*)buffer;
    Elf_Phdr* phdr_table = (Elf_Phdr*)((uint8_t*)buffer + ehdr->e_phoff);

    uintptr_t min_vaddr = (uintptr_t)-1, max_vaddr = 0;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type == PT_LOAD) {
            if (phdr_table[i].p_vaddr < min_vaddr) min_vaddr = phdr_table[i].p_vaddr;
            if (phdr_table[i].p_vaddr + phdr_table[i].p_memsz > max_vaddr) max_vaddr = phdr_table[i].p_vaddr + phdr_table[i].p_memsz;
        }
    }

    // Use PAGE_SIZE alignment
    size_t load_size = max_vaddr - min_vaddr;
    void* load_addr = mmap(NULL, load_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (load_addr == MAP_FAILED) return NULL;
    uintptr_t base = (uintptr_t)load_addr - min_vaddr;

    Elf_Dyn* dynamic = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type == PT_LOAD) {
            memcpy((void*)(base + phdr_table[i].p_vaddr), (uint8_t*)buffer + phdr_table[i].p_offset, phdr_table[i].p_filesz);
            if (phdr_table[i].p_memsz > phdr_table[i].p_filesz) {
                memset((uint8_t*)base + phdr_table[i].p_vaddr + phdr_table[i].p_filesz, 0, phdr_table[i].p_memsz - phdr_table[i].p_filesz);
            }
        } else if (phdr_table[i].p_type == PT_DYNAMIC) {
            dynamic = (Elf_Dyn*)(base + phdr_table[i].p_vaddr);
        }
    }

    Elf_Sym* symtab = NULL;
    const char* strtab = NULL;
    void* rel_data = NULL;
    size_t rel_size = 0;
    bool is_rela = false;
    void* init_array = NULL;
    size_t init_array_sz = 0;
    init_func_t init_func = NULL;

    for (Elf_Dyn* d = dynamic; d && d->d_tag != DT_NULL; d++) {
        switch (d->d_tag) {
            case DT_SYMTAB: symtab = (Elf_Sym*)(base + d->d_un.d_ptr); break;
            case DT_STRTAB: strtab = (const char*)(base + d->d_un.d_ptr); break;
            case DT_RELA: rel_data = (void*)(base + d->d_un.d_ptr); is_rela = true; break;
            case DT_RELASZ: rel_size = d->d_un.d_val; break;
            case DT_REL: rel_data = (void*)(base + d->d_un.d_ptr); is_rela = false; break;
            case DT_RELSZ: rel_size = d->d_un.d_val; break;
            case DT_INIT: init_func = (init_func_t)(base + d->d_un.d_ptr); break;
            case DT_INIT_ARRAY: init_array = (void*)(base + d->d_un.d_ptr); break;
            case DT_INIT_ARRAYSZ: init_array_sz = d->d_un.d_val; break;
        }
    }

    // Apply relocations
    if (rel_data) {
#ifdef __arm__
        Elf32_Rel* rel = (Elf32_Rel*)rel_data;
        for (size_t i = 0; i < rel_size / sizeof(Elf32_Rel); i++) {
            uintptr_t* ptr = (uintptr_t*)(base + rel[i].r_offset);
            int type = ELF32_R_TYPE(rel[i].r_info);
            if (type == REL_RELATIVE) *ptr += base;
            else if (type == REL_GLOB_DAT || type == REL_JUMP_SLOT || type == REL_ABS) {
                void* s = dlsym(RTLD_DEFAULT, strtab + symtab[ELF32_R_SYM(rel[i].r_info)].st_name);
                if (s) *ptr = (uintptr_t)s;
            }
        }
#else
        Elf64_Rela* rela = (Elf64_Rela*)rel_data;
        for (size_t i = 0; i < rel_size / sizeof(Elf64_Rela); i++) {
            uintptr_t* ptr = (uintptr_t*)(base + rela[i].r_offset);
            int type = ELF64_R_TYPE(rela[i].r_info);
            if (type == REL_RELATIVE) *ptr = base + rela[i].r_addend;
            else if (type == REL_GLOB_DAT || type == REL_JUMP_SLOT || type == REL_ABS) {
                void* s = dlsym(RTLD_DEFAULT, strtab + symtab[ELF64_R_SYM(rela[i].r_info)].st_name);
                if (s) *ptr = (uintptr_t)s + rela[i].r_addend;
            }
        }
#endif
    }

    // Set segment permissions
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type == PT_LOAD) {
            uintptr_t start = (base + phdr_table[i].p_vaddr) & ~4095UL;
            uintptr_t end = (base + phdr_table[i].p_vaddr + phdr_table[i].p_memsz + 4095UL) & ~4095UL;
            int prot = 0;
            if (phdr_table[i].p_flags & PF_R) prot |= PROT_READ;
            if (phdr_table[i].p_flags & PF_W) prot |= PROT_WRITE;
            if (phdr_table[i].p_flags & PF_X) prot |= PROT_EXEC;
            mprotect((void*)start, end - start, prot);
        }
    }

    // Call init functions
    if (init_func) init_func();
    if (init_array && init_array_sz > 0) {
        init_func_t* funcs = (init_func_t*)init_array;
        for (size_t i = 0; i < init_array_sz / sizeof(void*); i++) {
            if (funcs[i]) funcs[i]();
        }
    }

    return (void*)base;
}

void* vmp_find_symbol(void* handle, const char* name) {
    uintptr_t base = (uintptr_t)handle;
    Elf_Ehdr* ehdr = (Elf_Ehdr*)base;
    Elf_Phdr* phdr_table = (Elf_Phdr*)(base + ehdr->e_phoff);
    Elf_Dyn* dynamic = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type == PT_DYNAMIC) {
            dynamic = (Elf_Dyn*)(base + phdr_table[i].p_vaddr);
            break;
        }
    }
    if (!dynamic) return NULL;
    Elf_Sym* symtab = NULL;
    const char* strtab = NULL;
    for (Elf_Dyn* d = dynamic; d->d_tag != DT_NULL; d++) {
        if (d->d_tag == DT_SYMTAB) symtab = (Elf_Sym*)(base + d->d_un.d_ptr);
        if (d->d_tag == DT_STRTAB) strtab = (const char*)(base + d->d_un.d_ptr);
    }
    if (!symtab || !strtab) return NULL;
    for (int i = 0; i < 10000; i++) {
        if (strcmp(strtab + symtab[i].st_name, name) == 0) return (void*)(base + symtab[i].st_value);
    }
    return NULL;
}
