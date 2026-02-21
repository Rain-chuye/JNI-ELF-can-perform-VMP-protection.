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
        mData = (uint8_t*)malloc(mSize + 1024 * 1024); // Extra buffer
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
    is64Bit = (mData[EI_CLASS] == ELFCLASS64);
    if (is64Bit) header.ehdr64 = (Elf64_Ehdr*)mData;
    else header.ehdr32 = (Elf32_Ehdr*)mData;
    return true;
}

uintptr_t ElfParser::vaddrToOffset(uintptr_t vaddr) {
    if (is64Bit) {
        Elf64_Phdr* phdr = (Elf64_Phdr*)(mData + header.ehdr64->e_phoff);
        for (int i = 0; i < header.ehdr64->e_phnum; i++) {
            if (phdr[i].p_type == PT_LOAD && vaddr >= phdr[i].p_vaddr && vaddr < phdr[i].p_vaddr + phdr[i].p_memsz)
                return phdr[i].p_offset + (vaddr - phdr[i].p_vaddr);
        }
    } else {
        Elf32_Phdr* phdr = (Elf32_Phdr*)(mData + header.ehdr32->e_phoff);
        for (int i = 0; i < header.ehdr32->e_phnum; i++) {
            if (phdr[i].p_type == PT_LOAD && vaddr >= phdr[i].p_vaddr && vaddr < phdr[i].p_vaddr + phdr[i].p_memsz)
                return phdr[i].p_offset + (vaddr - phdr[i].p_vaddr);
        }
    }
    return 0;
}

uint8_t* ElfParser::getSection(const char* name, size_t* outSize) {
    if (is64Bit) {
        Elf64_Shdr* shdr = (Elf64_Shdr*)(mData + header.ehdr64->e_shoff);
        const char* strtab = (const char*)(mData + shdr[header.ehdr64->e_shstrndx].sh_offset);
        for (int i = 0; i < header.ehdr64->e_shnum; i++) {
            if (strcmp(strtab + shdr[i].sh_name, name) == 0) {
                if (outSize) *outSize = shdr[i].sh_size;
                return mData + shdr[i].sh_offset;
            }
        }
    } else {
        Elf32_Shdr* shdr = (Elf32_Shdr*)(mData + header.ehdr32->e_shoff);
        const char* strtab = (const char*)(mData + shdr[header.ehdr32->e_shstrndx].sh_offset);
        for (int i = 0; i < header.ehdr32->e_shnum; i++) {
            if (strcmp(strtab + shdr[i].sh_name, name) == 0) {
                if (outSize) *outSize = shdr[i].sh_size;
                return mData + shdr[i].sh_offset;
            }
        }
    }
    return NULL;
}

bool ElfParser::patchSection(const char* name, const uint8_t* data, size_t size) {
    size_t secSize = 0;
    uint8_t* secPtr = getSection(name, &secSize);
    if (secPtr && secSize >= size) {
        memcpy(secPtr, data, size);
        return true;
    }
    return false;
}

bool ElfParser::save(const char* path) {
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;
    fwrite(mData, 1, mSize, fp);
    fclose(fp);
    return true;
}
