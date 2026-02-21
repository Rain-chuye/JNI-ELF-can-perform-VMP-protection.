#ifndef ELF_PARSER_H
#define ELF_PARSER_H

#include <stdint.h>
#include <vector>
#include <string>
#include <elf.h>

class ElfParser {
public:
    ElfParser(const char* path);
    ~ElfParser();

    bool parse();
    bool save(const char* path);
    uint8_t* getSection(const char* name, size_t* outSize);
    uintptr_t getSymbolOffset(const char* name);
    uintptr_t vaddrToOffset(uintptr_t vaddr);

    union {
        Elf32_Ehdr* ehdr32;
        Elf64_Ehdr* ehdr64;
    } header;

    bool is64Bit;
    uint8_t* mData;
    size_t mSize;
};

#endif // ELF_PARSER_H
