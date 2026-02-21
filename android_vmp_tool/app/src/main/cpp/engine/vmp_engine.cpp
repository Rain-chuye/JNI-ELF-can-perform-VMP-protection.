#include "vmp_engine.h"
#include <android/log.h>
#include <string.h>
#include <vector>
#include <set>

#define LOG_TAG "VMP_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

VmpEngine::VmpEngine(ElfParser* parser) : mParser(parser) {}

void VmpEngine::protect() {
    LOGI("Relocation-Aware Protection Starting...");

    std::set<uintptr_t> untouchable;

    // 1. COLLECT RELOCATION TARGETS
    auto collectRelocs = [&](const char* sectionName) {
        size_t size = 0;
        uint8_t* data = mParser->getSection(sectionName, &size);
        if (!data) return;

        if (mParser->is64Bit) {
            Elf64_Rela* rel = (Elf64_Rela*)data;
            for (size_t i = 0; i < size / sizeof(Elf64_Rela); i++) {
                untouchable.insert(rel[i].r_offset);
            }
        } else {
            Elf32_Rel* rel = (Elf32_Rel*)data;
            for (size_t i = 0; i < size / sizeof(Elf32_Rel); i++) {
                untouchable.insert(rel[i].r_offset);
            }
        }
    };

    collectRelocs(".rel.dyn");
    collectRelocs(".rel.plt");
    collectRelocs(".rela.dyn");
    collectRelocs(".rela.plt");

    LOGI("Found %zu relocation targets. These will be skipped to avoid crashes.", untouchable.size());

    // 2. ENCRYPT .rodata SAFELY
    size_t roSize = 0;
    uint8_t* rodata = mParser->getSection(".rodata", &roSize);
    uintptr_t roVaddr = 0;
    // Find .rodata vaddr
    if (mParser->is64Bit) {
        Elf64_Shdr* sh = (Elf64_Shdr*)(mParser->mData + mParser->header.ehdr64->e_shoff);
        for(int i=0; i<mParser->header.ehdr64->e_shnum; i++) {
             if (sh[i].sh_type == SHT_PROGBITS && sh[i].sh_size == roSize) { roVaddr = sh[i].sh_addr; break; }
        }
    } else {
        Elf32_Shdr* sh = (Elf32_Shdr*)(mParser->mData + mParser->header.ehdr32->e_shoff);
        for(int i=0; i<mParser->header.ehdr32->e_shnum; i++) {
             if (sh[i].sh_type == SHT_PROGBITS && sh[i].sh_size == roSize) { roVaddr = sh[i].sh_addr; break; }
        }
    }

    if (rodata) {
        for (size_t i = 0; i < roSize; i++) {
            if (untouchable.find(roVaddr + i) == untouchable.end()) {
                rodata[i] ^= 0x42;
            }
        }
    }

    // 3. ENCRYPT JNI EXPORTS SAFELY
    size_t symSize = 0;
    uint8_t* dynsym = mParser->getSection(".dynsym", &symSize);
    uint8_t* dynstr = mParser->getSection(".dynstr", NULL);

    if (dynsym && dynstr) {
        if (mParser->is64Bit) {
            Elf64_Sym* syms = (Elf64_Sym*)dynsym;
            for (int i = 0; i < symSize / sizeof(Elf64_Sym); i++) {
                const char* name = (const char*)dynstr + syms[i].st_name;
                if (strncmp(name, "Java_", 5) == 0) {
                    uintptr_t offset = mParser->vaddrToOffset(syms[i].st_value);
                    if (offset > 0) {
                        for (size_t j = 16; j < syms[i].st_size; j++) {
                            if (untouchable.find(syms[i].st_value + j) == untouchable.end())
                                mParser->mData[offset + j] ^= 0x77;
                        }
                    }
                }
            }
        } else {
            Elf32_Sym* syms = (Elf32_Sym*)dynsym;
            for (int i = 0; i < symSize / sizeof(Elf32_Sym); i++) {
                const char* name = (const char*)dynstr + syms[i].st_name;
                if (strncmp(name, "Java_", 5) == 0) {
                    uintptr_t vaddr = syms[i].st_value & ~1;
                    uintptr_t offset = mParser->vaddrToOffset(vaddr);
                    if (offset > 0) {
                        for (size_t j = 8; j < syms[i].st_size; j++) {
                            if (untouchable.find(vaddr + j) == untouchable.end())
                                mParser->mData[offset + j] ^= 0x33;
                        }
                    }
                }
            }
        }
    }
    LOGI("Protection complete with relocation safety.");
}
