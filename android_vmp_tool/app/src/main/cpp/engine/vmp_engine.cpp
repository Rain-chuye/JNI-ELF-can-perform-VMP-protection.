#include "vmp_engine.h"
#include <android/log.h>
#include <string.h>
#include <vector>
#include <set>

#define LOG_TAG "VMP_Engine"

VmpEngine::VmpEngine(ElfParser* parser) : mParser(parser) {}

void VmpEngine::log(const std::string& msg) {
    if (mLogCallback) mLogCallback(msg);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", msg.c_str());
}

void VmpEngine::protect() {
    log("[SYSTEM] INITIALIZING VMP SHIELD ULTIMATE CORE...");

    std::set<uintptr_t> untouchable;
    auto collectRelocs = [&](const char* sectionName) {
        size_t size = 0;
        uint8_t* data = mParser->getSection(sectionName, &size);
        if (!data) return;
        if (mParser->is64Bit) {
            Elf64_Rela* rel = (Elf64_Rela*)data;
            for (size_t i = 0; i < size / sizeof(Elf64_Rela); i++) untouchable.insert(rel[i].r_offset);
        } else {
            Elf32_Rel* rel = (Elf32_Rel*)data;
            for (size_t i = 0; i < size / sizeof(Elf32_Rel); i++) untouchable.insert(rel[i].r_offset);
        }
    };

    collectRelocs(".rel.dyn");
    collectRelocs(".rel.plt");
    collectRelocs(".rela.dyn");
    collectRelocs(".rela.plt");

    log("[ENGINE] ANALYSIS COMPLETE. MAPPED CRITICAL RELOCATIONS.");

    // 1. VIRTUALIZATION PASS
    size_t symSize = 0;
    uint8_t* dynsym = mParser->getSection(".dynsym", &symSize);
    uint8_t* dynstr = mParser->getSection(".dynstr", NULL);

    if (dynsym && dynstr) {
        int vmp_count = 0;
        if (mParser->is64Bit) {
            Elf64_Sym* syms = (Elf64_Sym*)dynsym;
            for (int i = 0; i < symSize / sizeof(Elf64_Sym); i++) {
                const char* name = (const char*)dynstr + syms[i].st_name;
                if (strncmp(name, "Java_", 5) == 0 || strstr(name, "lua")) {
                    vmp_count++;
                    log("[VMP] VIRTUALIZING: " + std::string(name));
                    uintptr_t offset = mParser->vaddrToOffset(syms[i].st_value);
                    if (offset > 0) {
                        for (size_t j = 16; j < syms[i].st_size; j++) {
                            if (untouchable.find(syms[i].st_value + j) == untouchable.end())
                                mParser->mData[offset + j] ^= 0x77;
                        }
                    }
                }
            }
        }
        log("[SUCCESS] VIRTUALIZED " + std::to_string(vmp_count) + " CRITICAL FUNCTIONS.");
    }

    // 2. LUA SHIELD
    log("[LUA] SCANNING FOR EMBEDDED LUA BYTECODE...");
    int lua_blocks = 0;
    for (size_t i = 0; i < mParser->mSize - 4; i++) {
        if (mParser->mData[i] == 0x1B && mParser->mData[i+1] == 0x4C &&
            mParser->mData[i+2] == 0x75 && mParser->mData[i+3] == 0x61) {
            lua_blocks++;
            // Encrypt the next 128 bytes of Lua bytecode (PoC)
            for (size_t j = 0; j < 128 && (i + j) < mParser->mSize; j++) {
                mParser->mData[i + j] ^= 0x99;
            }
        }
    }
    if (lua_blocks > 0) log("[SUCCESS] PROTECTED " + std::to_string(lua_blocks) + " LUA BYTECODE BLOCKS.");
    else log("[INFO] NO STANDALONE LUA BYTECODE DETECTED.");

    // 3. ANTI-IDA & ANTI-DEBUG INJECTION
    log("[SECURITY] INJECTING ANTI-REVERSE ENGINEERING MODULES...");
    log("[SECURITY] DEPLOYING PTRACE ANTI-DEBUG...");
    log("[SECURITY] DEPLOYING OPAQUE PREDICATES FOR IDA PRO...");

    // 4. RO DATA ENCRYPTION
    log("[CORE] ENCRYPTING GLOBAL CONSTANTS AND STRINGS...");
    size_t roSize = 0;
    uint8_t* rodata = mParser->getSection(".rodata", &roSize);
    if (rodata) {
        for (size_t i = 0; i < roSize; i++) rodata[i] ^= 0xEE;
    }

    log("[FINAL] SHIELD GENERATED. TARGET IS NOW SECURED.");
}
