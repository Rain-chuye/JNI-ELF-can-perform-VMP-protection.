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
        mData = (uint8_t*)malloc(mSize + 1024 * 1024);
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
        Elf64_Ehdr* ehdr = header.ehdr64;
        Elf64_Shdr* shdr = (Elf64_Shdr*)(mData + ehdr->e_shoff);
        const char* strtab = (const char*)(mData + shdr[ehdr->e_shstrndx].sh_offset);
        for (int i = 0; i < ehdr->e_shnum; i++) {
            if (strcmp(strtab + shdr[i].sh_name, name) == 0) {
                if (outSize) *outSize = shdr[i].sh_size;
                return mData + shdr[i].sh_offset;
            }
        }
    } else {
        Elf32_Ehdr* ehdr = header.ehdr32;
        Elf32_Shdr* shdr = (Elf32_Shdr*)(mData + ehdr->e_shoff);
        const char* strtab = (const char*)(mData + shdr[ehdr->e_shstrndx].sh_offset);
        for (int i = 0; i < ehdr->e_shnum; i++) {
            if (strcmp(strtab + shdr[i].sh_name, name) == 0) {
                if (outSize) *outSize = shdr[i].sh_size;
                return mData + shdr[i].sh_offset;
            }
        }
    }
    return NULL;
}

std::vector<JniExport> ElfParser::getJniExports() {
    std::vector<JniExport> exports;
    size_t symSize = 0;
    uint8_t* dynsym = getSection(".dynsym", &symSize);
    uint8_t* dynstr = getSection(".dynstr", NULL);
    if (!dynsym || !dynstr) return exports;

    if (is64Bit) {
        Elf64_Sym* syms = (Elf64_Sym*)dynsym;
        for (int i = 0; i < symSize / sizeof(Elf64_Sym); i++) {
            const char* name = (const char*)dynstr + syms[i].st_name;
            if (strncmp(name, "Java_", 5) == 0) {
                exports.push_back({name, (uintptr_t)syms[i].st_value, (size_t)syms[i].st_size});
            }
        }
    } else {
        Elf32_Sym* syms = (Elf32_Sym*)dynsym;
        for (int i = 0; i < symSize / sizeof(Elf32_Sym); i++) {
            const char* name = (const char*)dynstr + syms[i].st_name;
            if (strncmp(name, "Java_", 5) == 0) {
                exports.push_back({name, (uintptr_t)syms[i].st_value, (size_t)syms[i].st_size});
            }
        }
    }
    return exports;
}

bool ElfParser::save(const char* path) {
    FILE* fp = fopen(path, "wb");
    if (!fp) return false;
    fwrite(mData, 1, mSize, fp);
    fclose(fp);
    return true;
}
