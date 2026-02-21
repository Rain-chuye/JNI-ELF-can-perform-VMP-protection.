#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <dlfcn.h>

#ifdef __x86_64__
#define Elf_Ehdr Elf64_Ehdr
#define Elf_Phdr Elf64_Phdr
#define Elf_Dyn  Elf64_Dyn
#define Elf_Sym  Elf64_Sym
#define Elf_Rela Elf64_Rela
#define ELF_R_TYPE ELF64_R_TYPE
#define ELF_R_SYM  ELF64_R_SYM
#define REL_TYPE   R_X86_64_RELATIVE
#define GLOB_DAT   R_X86_64_GLOB_DAT
#define JUMP_SLOT  R_X86_64_JUMP_SLOT
#else
#define Elf_Ehdr Elf32_Ehdr
#define Elf_Phdr Elf32_Phdr
#define Elf_Dyn  Elf32_Dyn
#define Elf_Sym  Elf32_Sym
#define Elf_Rel  Elf32_Rel
#define ELF_R_TYPE ELF32_R_TYPE
#define ELF_R_SYM  ELF32_R_SYM
#endif

class CustomLinker {
public:
    void* base;
    Elf_Ehdr* ehdr;
    Elf_Phdr* phdr_table;
    Elf_Dyn* dynamic;
    Elf_Sym* symtab;
    const char* strtab;

    CustomLinker() : base(NULL), ehdr(NULL), phdr_table(NULL), dynamic(NULL), symtab(NULL), strtab(NULL) {}

    bool load(const char* path) {
        int fd = open(path, O_RDONLY);
        if (fd < 0) return false;

        Elf_Ehdr temp_ehdr;
        read(fd, &temp_ehdr, sizeof(temp_ehdr));

        size_t min_vaddr = (size_t)-1, max_vaddr = 0;
        lseek(fd, temp_ehdr.e_phoff, SEEK_SET);
        for (int i = 0; i < temp_ehdr.e_phnum; i++) {
            Elf_Phdr phdr;
            read(fd, &phdr, sizeof(phdr));
            if (phdr.p_type == PT_LOAD) {
                if (phdr.p_vaddr < min_vaddr) min_vaddr = phdr.p_vaddr;
                if (phdr.p_vaddr + phdr.p_memsz > max_vaddr) max_vaddr = phdr.p_vaddr + phdr.p_memsz;
            }
        }

        size_t load_size = max_vaddr - min_vaddr;
        base = mmap(NULL, load_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        uintptr_t load_base = (uintptr_t)base - min_vaddr;

        lseek(fd, temp_ehdr.e_phoff, SEEK_SET);
        for (int i = 0; i < temp_ehdr.e_phnum; i++) {
            Elf_Phdr phdr;
            read(fd, &phdr, sizeof(phdr));
            if (phdr.p_type == PT_LOAD) {
                lseek(fd, phdr.p_offset, SEEK_SET);
                read(fd, (void*)(load_base + phdr.p_vaddr), phdr.p_filesz);
            } else if (phdr.p_type == PT_DYNAMIC) {
                dynamic = (Elf_Dyn*)(load_base + phdr.p_vaddr);
            }
        }
        close(fd);

        ehdr = (Elf_Ehdr*)base; // Note: this is actually the mapped start, might not be same as ehdr if min_vaddr > 0

        // Parse Dynamic
        Elf_Rela* rela = NULL;
        size_t rela_size = 0;

        for (Elf_Dyn* d = dynamic; d && d->d_tag != DT_NULL; d++) {
            if (d->d_tag == DT_RELA) rela = (Elf_Rela*)(load_base + d->d_un.d_ptr);
            if (d->d_tag == DT_RELASZ) rela_size = d->d_un.d_val;
            if (d->d_tag == DT_SYMTAB) symtab = (Elf_Sym*)(load_base + d->d_un.d_ptr);
            if (d->d_tag == DT_STRTAB) strtab = (const char*)(load_base + d->d_un.d_ptr);
        }

        if (rela) {
            for (size_t i = 0; i < rela_size / sizeof(Elf_Rela); i++) {
                uintptr_t* rel_addr = (uintptr_t*)(load_base + rela[i].r_offset);
                int type = ELF_R_TYPE(rela[i].r_info);
                int sym_idx = ELF_R_SYM(rela[i].r_info);

                if (type == REL_TYPE) {
                    *rel_addr = load_base + rela[i].r_addend;
                } else if (type == GLOB_DAT || type == JUMP_SLOT) {
                    const char* sym_name = strtab + symtab[sym_idx].st_name;
                    void* sym_ptr = dlsym(RTLD_DEFAULT, sym_name);
                    if (sym_ptr) *rel_addr = (uintptr_t)sym_ptr;
                }
            }
        }
        return true;
    }

    void* get_symbol(const char* name) {
        if (!symtab || !strtab) return NULL;
        // Simple linear search for testing
        for (int i = 0; ; i++) {
            const char* sym_name = strtab + symtab[i].st_name;
            if (strcmp(sym_name, name) == 0) {
                uintptr_t load_base = (uintptr_t)base; // Simplified, assuming min_vaddr=0
                return (void*)(load_base + symtab[i].st_value);
            }
            if (symtab[i].st_info == 0 && i > 0) break; // End of symtab? (Hack)
            if (i > 1000) break;
        }
        return NULL;
    }
};

int main() {
    CustomLinker linker;
    if (!linker.load("./test_env/libtarget.so")) {
        printf("Load failed\n");
        return 1;
    }
    printf("Library loaded successfully\n");

    typedef void (*hello_t)();
    hello_t hello = (hello_t)linker.get_symbol("hello");
    if (hello) {
        hello();
    } else {
        printf("Symbol 'hello' not found\n");
    }

    typedef int (*add_t)(int, int);
    add_t add = (add_t)linker.get_symbol("add");
    if (add) {
        printf("add(5, 7) = %d\n", add(5, 7));
    }

    return 0;
}
