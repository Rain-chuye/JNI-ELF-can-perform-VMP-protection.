#include "elf_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

ElfParser::ElfParser(const char* path) : mData(NULL), mSize(0), is64Bit(false) {
    FILE* fp = fopen(path, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        mSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        mData = (uint8_t*)malloc(mSize + 4096); // Extra space for injection
        memset(mData, 0, mSize + 4096);
        fread(mData, 1, mSize, fp);
        fclose(fp);
    }
}

ElfParser::~ElfParser() {
    if (mData) free(mData);
}

bool ElfParser::parse() {
    if (!mData || mSize < sizeof(Elf32_Ehdr)) return false;
    if (memcmp(mData, ELFMAG, SELFMAG) != 0) return false;

    if (mData[EI_CLASS] == ELFCLASS64) {
        is64Bit = true;
        header.ehdr64 = (Elf64_Ehdr*)mData;
    } else {
        is64Bit = false;
        header.ehdr32 = (Elf32_Ehdr*)mData;
    }
    return true;
}

uint8_t* ElfParser::getSection(const char* name, size_t* outSize) {
    if (is64Bit) {
        Elf64_Ehdr* ehdr = header.ehdr64;
        Elf64_Shdr* shdr_table = (Elf64_Shdr*)(mData + ehdr->e_shoff);
        Elf64_Shdr* shstrtab_hdr = &shdr_table[ehdr->e_shstrndx];
        const char* shstrtab = (const char*)(mData + shstrtab_hdr->sh_offset);

        for (int i = 0; i < ehdr->e_shnum; i++) {
            if (strcmp(shstrtab + shdr_table[i].sh_name, name) == 0) {
                if (outSize) *outSize = shdr_table[i].sh_size;
                return mData + shdr_table[i].sh_offset;
            }
        }
    } else {
        Elf32_Ehdr* ehdr = header.ehdr32;
        Elf32_Shdr* shdr_table = (Elf32_Shdr*)(mData + ehdr->e_shoff);
        Elf32_Shdr* shstrtab_hdr = &shdr_table[ehdr->e_shstrndx];
        const char* shstrtab = (const char*)(mData + shstrtab_hdr->sh_offset);

        for (int i = 0; i < ehdr->e_shnum; i++) {
            if (strcmp(shstrtab + shdr_table[i].sh_name, name) == 0) {
                if (outSize) *outSize = shdr_table[i].sh_size;
                return mData + shdr_table[i].sh_offset;
            }
        }
    }
    return NULL;
}

bool ElfParser::save(const char* path) {
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;
    fwrite(mData, 1, mSize, fp);
    fclose(fp);
    return true;
}
