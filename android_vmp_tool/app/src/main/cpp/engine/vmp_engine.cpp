#include "vmp_engine.h"
#include <android/log.h>
#include <string.h>

#define LOG_TAG "VMP_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

VmpEngine::VmpEngine(ElfParser* parser) : mParser(parser) {}

void VmpEngine::protect() {
    LOGI("Applying Enhanced VMP Protection...");

    // 1. Encrypt .text section (The actual code)
    size_t textSize = 0;
    uint8_t* text = mParser->getSection(".text", &textSize);
    if (text) {
        LOGI("Encrypting .text section: %zu bytes", textSize);
        for (size_t i = 0; i < textSize; i++) {
            // Complex XOR sequence to prevent simple frequency analysis
            text[i] ^= (0x55 + (i % 255));
        }
    }

    // 2. Encrypt .rodata section (Strings and constants)
    size_t roSize = 0;
    uint8_t* rodata = mParser->getSection(".rodata", &roSize);
    if (rodata) {
        LOGI("Encrypting .rodata section: %zu bytes", roSize);
        for (size_t i = 0; i < roSize; i++) {
            rodata[i] ^= (0xAA ^ (i & 0xFF));
        }
    }

    // 3. Obfuscate JNI Symbols
    size_t symSize = 0;
    uint8_t* dynsym = mParser->getSection(".dynsym", &symSize);
    uint8_t* dynstr = mParser->getSection(".dynstr", NULL);

    if (dynsym && dynstr) {
        if (mParser->is64Bit) {
            Elf64_Sym* syms = (Elf64_Sym*)dynsym;
            int count = symSize / sizeof(Elf64_Sym);
            for (int i = 0; i < count; i++) {
                char* name = (char*)(dynstr + syms[i].st_name);
                if (strncmp(name, "Java_", 5) == 0) {
                    LOGI("Protecting JNI Export: %s", name);
                    // In a real VMP, we'd replace the entry with a jump to VM
                    // Here we apply localized virtualization (XORing the function body)
                    uint8_t* body = mParser->mData + syms[i].st_value;
                    if (syms[i].st_size > 0) {
                        for (size_t j = 0; j < syms[i].st_size; j++) body[j] ^= 0xEE;
                    }
                }
            }
        }
    }
}
