#ifndef JNI_PROTECTOR_H
#define JNI_PROTECTOR_H

#include "elf_parser.h"

class JniProtector {
private: ElfParser* mParser;
public:
    JniProtector(ElfParser* parser);
    void protect();
};

#endif // JNI_PROTECTOR_H
