#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <sys/mman.h>

void protect(const char* in, const char* out) {
    FILE* f = fopen(in, "rb");
    fseek(f, 0, SEEK_END);
    size_t sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    uint8_t* data = (uint8_t*)malloc(sz);
    fread(data, 1, sz, f);
    fclose(f);

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)data;
    Elf64_Shdr* shdr = (Elf64_Shdr*)(data + ehdr->e_shoff);
    Elf64_Shdr* shstrtab = &shdr[ehdr->e_shstrndx];
    const char* names = (const char*)(data + shstrtab->sh_offset);

    for (int i = 0; i < ehdr->e_shnum; i++) {
        if (strcmp(names + shdr[i].sh_name, ".rodata") == 0) {
            printf("Encrypting .rodata\n");
            for (size_t j = 0; j < shdr[i].sh_size; j++) {
                data[shdr[i].sh_offset + j] ^= 0x42;
            }
        }
    }

    f = fopen(out, "wb");
    fwrite(data, 1, sz, f);
    fclose(f);
}

int main() {
    protect("./test_env/libtarget.so", "./test_env/libtarget_protected.so");
    return 0;
}
