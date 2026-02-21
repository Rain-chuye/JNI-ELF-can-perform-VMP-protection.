#include "elf_parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ElfParser::ElfParser(const char* path) : mData(NULL), mSize(0), is64Bit(false) {
    FILE* fp = fopen(path, "rb");
    if (fp) {
        fseek(fp, 0, SEEK_END);
        mSize = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        mData = (uint8_t*)malloc(mSize);
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
        Elf64_Shdr* shstrtab_hdr = (Elf64_Shdr*)(mData + ehdr->e_shoff + ehdr->e_shstrndx * ehdr->e_shentsize);
        const char* shstrtab = (const char*)(mData + shstrtab_hdr->sh_offset);

        for (int i = 0; i < ehdr->e_shnum; i++) {
            Elf64_Shdr* shdr = (Elf64_Shdr*)(mData + ehdr->e_shoff + i * ehdr->e_shentsize);
            if (strcmp(shstrtab + shdr->sh_name, name) == 0) {
                if (outSize) *outSize = shdr->sh_size;
                return mData + shdr->sh_offset;
            }
        }
    } else {
        Elf32_Ehdr* ehdr = header.ehdr32;
        Elf32_Shdr* shstrtab_hdr = (Elf32_Shdr*)(mData + ehdr->e_shoff + ehdr->e_shstrndx * ehdr->e_shentsize);
        const char* shstrtab = (const char*)(mData + shstrtab_hdr->sh_offset);

        for (int i = 0; i < ehdr->e_shnum; i++) {
            Elf32_Shdr* shdr = (Elf32_Shdr*)(mData + ehdr->e_shoff + i * ehdr->e_shentsize);
            if (strcmp(shstrtab + shdr->sh_name, name) == 0) {
                if (outSize) *outSize = shdr->sh_size;
                return mData + shdr->sh_offset;
            }
        }
    }
    return NULL;
}

uintptr_t ElfParser::getSymbolOffset(const char* name) {
    size_t dynsymSize, dynstrSize;
    uint8_t* dynsym = getSection(".dynsym", &dynsymSize);
    uint8_t* dynstr = getSection(".dynstr", &dynstrSize);

    if (!dynsym || !dynstr) return 0;

    if (is64Bit) {
        Elf64_Sym* syms = (Elf64_Sym*)dynsym;
        int count = dynsymSize / sizeof(Elf64_Sym);
        for (int i = 0; i < count; i++) {
            if (strcmp((const char*)dynstr + syms[i].st_name, name) == 0) {
                return syms[i].st_value;
            }
        }
    } else {
        Elf32_Sym* syms = (Elf32_Sym*)dynsym;
        int count = dynsymSize / sizeof(Elf32_Sym);
        for (int i = 0; i < count; i++) {
            if (strcmp((const char*)dynstr + syms[i].st_name, name) == 0) {
                return syms[i].st_value;
            }
        }
    }
    return 0;
}

bool ElfParser::save(const char* path) {
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;
    fwrite(mData, 1, mSize, fp);
    fclose(fp);
    return true;
}
