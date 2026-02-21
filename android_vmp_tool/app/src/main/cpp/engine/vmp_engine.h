#ifndef VMP_ENGINE_H
#define VMP_ENGINE_H

#include "elf_parser.h"

class VmpEngine {
public:
    VmpEngine(ElfParser* parser);
    void protect();

private:
    ElfParser* mParser;
    void virtualizeFunction(uintptr_t offset, size_t size);
};

#endif // VMP_ENGINE_H
