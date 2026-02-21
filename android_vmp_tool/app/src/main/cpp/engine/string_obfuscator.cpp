#include "string_obfuscator.h"
#include <android/log.h>
#include <string.h>

#define LOG_TAG "StringObfuscator"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

StringObfuscator::StringObfuscator(ElfParser* parser) : mParser(parser) {}

void StringObfuscator::obfuscate() {
    LOGI("Starting string obfuscation...");
    size_t size = 0;
    uint8_t* rodata = mParser->getSection(".rodata", &size);
    if (rodata) {
        LOGI("Found .rodata section, size: %zu. Encrypting...", size);
        for (size_t i = 0; i < size; i++) {
            // Simple XOR encryption for PoC
            rodata[i] ^= 0x42;
        }
    } else {
        LOGI("No .rodata section found.");
    }
}
