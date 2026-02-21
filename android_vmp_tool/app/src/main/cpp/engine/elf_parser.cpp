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
        mData = (uint8_t*)malloc(mSize + 4096);
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

uintptr_t ElfParser::vaddrToOffset(uintptr_t vaddr) {
    if (is64Bit) {
        Elf64_Phdr* phdr = (Elf64_Phdr*)(mData + header.ehdr64->e_phoff);
        for (int i = 0; i < header.ehdr64->e_phnum; i++) {
            if (phdr[i].p_type == PT_LOAD) {
                if (vaddr >= phdr[i].p_vaddr && vaddr < phdr[i].p_vaddr + phdr[i].p_memsz) {
                    return phdr[i].p_offset + (vaddr - phdr[i].p_vaddr);
                }
            }
        }
    } else {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(mData + header.ehdr32->e_phoff);
        for (int i = 0; i < header.ehdr32->e_phnum; i++) {
            if (phdr[i].p_type == PT_LOAD) {
                if (vaddr >= phdr[i].p_vaddr && vaddr < phdr[i].p_vaddr + phdr[i].p_memsz) {
                    return phdr[i].p_offset + (vaddr - phdr[i].p_vaddr);
                }
            }
        }
    }
    return 0;
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
