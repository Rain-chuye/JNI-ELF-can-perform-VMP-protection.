#ifndef STRING_OBFUSCATOR_H
#define STRING_OBFUSCATOR_H

#include "elf_parser.h"

class StringObfuscator {
private: ElfParser* mParser;
public:
    StringObfuscator(ElfParser* parser);
    void obfuscate();
};

#endif // STRING_OBFUSCATOR_H
