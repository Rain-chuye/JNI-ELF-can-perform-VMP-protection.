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
#define R_RELATIVE R_ARM_RELATIVE
#define R_GLOB_DAT R_ARM_GLOB_DAT
#define R_JUMP_SLOT R_ARM_JUMP_SLOT
#else
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Sym  Elf64_Sym
#define Elf_Dyn  Elf64_Dyn
#define Elf_Rela Elf64_Rela
#define ELF_R_SYM(i) ELF64_R_SYM(i)
#define ELF_R_TYPE(i) ELF64_R_TYPE(i)
#define R_RELATIVE R_AARCH64_RELATIVE
#define R_GLOB_DAT R_AARCH64_GLOB_DAT
#define R_JUMP_SLOT R_AARCH64_JUMP_SLOT
#endif

void* vmp_load_library_from_mem(void* buffer, size_t size) {
    Elf_Ehdr* ehdr = (Elf_Ehdr*)buffer;
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) return NULL;

    // 1. Calculate and map segments
    Elf_Phdr* phdr_table = (Elf_Phdr*)((uint8_t*)buffer + ehdr->e_phoff);
    uintptr_t min_vaddr = (uintptr_t)-1, max_vaddr = 0;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type == PT_LOAD) {
            if (phdr_table[i].p_vaddr < min_vaddr) min_vaddr = phdr_table[i].p_vaddr;
            if (phdr_table[i].p_vaddr + phdr_table[i].p_memsz > max_vaddr) max_vaddr = phdr_table[i].p_vaddr + phdr_table[i].p_memsz;
        }
    }

    void* load_addr = mmap(NULL, max_vaddr - min_vaddr, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (load_addr == MAP_FAILED) return NULL;
    uintptr_t base = (uintptr_t)load_addr - min_vaddr;

    Elf_Dyn* dynamic = NULL;
    for (int i = 0; i < ehdr->e_phnum; i++) {
        if (phdr_table[i].p_type == PT_LOAD) {
            memcpy((void*)(base + phdr_table[i].p_vaddr), (uint8_t*)buffer + phdr_table[i].p_offset, phdr_table[i].p_filesz);
        } else if (phdr_table[i].p_type == PT_DYNAMIC) {
            dynamic = (Elf_Dyn*)(base + phdr_table[i].p_vaddr);
        }
    }

    // 2. Comprehensive Relocations
    Elf_Sym* symtab = NULL;
    const char* strtab = NULL;
    void* rel_data = NULL;
    size_t rel_size = 0;
    bool is_rela = false;

    for (Elf_Dyn* d = dynamic; d && d->d_tag != DT_NULL; d++) {
        switch (d->d_tag) {
            case DT_SYMTAB: symtab = (Elf_Sym*)(base + d->d_un.d_ptr); break;
            case DT_STRTAB: strtab = (const char*)(base + d->d_un.d_ptr); break;
            case DT_RELA: rel_data = (void*)(base + d->d_un.d_ptr); is_rela = true; break;
            case DT_RELASZ: rel_size = d->d_un.d_val; break;
            case DT_REL: rel_data = (void*)(base + d->d_un.d_ptr); is_rela = false; break;
            case DT_RELSZ: rel_size = d->d_un.d_val; break;
        }
    }

    if (rel_data) {
#ifdef __arm__
        Elf32_Rel* rel = (Elf32_Rel*)rel_data;
        for (size_t i = 0; i < rel_size / sizeof(Elf32_Rel); i++) {
            uintptr_t* ptr = (uintptr_t*)(base + rel[i].r_offset);
            int type = ELF32_R_TYPE(rel[i].r_info);
            int sym = ELF32_R_SYM(rel[i].r_info);
            if (type == R_RELATIVE) *ptr += base;
            else if (type == R_GLOB_DAT || type == R_JUMP_SLOT) {
                void* s = dlsym(RTLD_DEFAULT, strtab + symtab[sym].st_name);
                if (s) *ptr = (uintptr_t)s;
            }
        }
#else
        Elf64_Rela* rela = (Elf64_Rela*)rel_data;
        for (size_t i = 0; i < rel_size / sizeof(Elf64_Rela); i++) {
            uintptr_t* ptr = (uintptr_t*)(base + rela[i].r_offset);
            int type = ELF64_R_TYPE(rela[i].r_info);
            int sym = ELF64_R_SYM(rela[i].r_info);
            if (type == R_RELATIVE) *ptr = base + rela[i].r_addend;
            else if (type == R_GLOB_DAT || type == R_JUMP_SLOT) {
                void* s = dlsym(RTLD_DEFAULT, strtab + symtab[sym].st_name);
                if (s) *ptr = (uintptr_t)s;
            }
        }
#endif
    }

    LOGI("Linker: Library successfully mapped and relocated at %p", load_addr);
    return load_addr;
}

void* vmp_get_symbol_from_lib(void* handle, const char* name) {
    // Note: handle here is the load_addr from vmp_load_library_from_mem
    // We need to find the symbol in its symtab.
    // For simplicity, we just return NULL and let JNI resolution handle it,
    // OR we could implement a full symtab search.
    return NULL;
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

    // Linear search (PoC)
    for (int i = 0; ; i++) {
        if (strcmp(strtab + symtab[i].st_name, name) == 0) {
            return (void*)(base + symtab[i].st_value);
        }
        if (i > 5000) break; // Safety break
    }
    return NULL;
}
