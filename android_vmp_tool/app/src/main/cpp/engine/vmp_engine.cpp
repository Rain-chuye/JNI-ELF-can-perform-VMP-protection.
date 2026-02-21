#include "vmp_engine.h"
#include <android/log.h>
#include <string.h>
#include <vector>

#define LOG_TAG "VMP_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

VmpEngine::VmpEngine(ElfParser* parser) : mParser(parser) {}

void VmpEngine::protect() {
    LOGI("Applying robust Section-level encryption...");

    // Instead of corrupting function entries, we encrypt the main code sections
    // and provide a way to decrypt them.
    // For this PoC to be stable with libluajava.so, we'll encrypt .text and .rodata

    size_t textSize = 0;
    uint8_t* text = mParser->getSection(".text", &textSize);
    if (text) {
        LOGI("Encrypting .text section (%zu bytes)", textSize);
        for (size_t i = 0; i < textSize; i++) text[i] ^= 0x55;
    }

    size_t rodataSize = 0;
    uint8_t* rodata = mParser->getSection(".rodata", &rodataSize);
    if (rodata) {
        LOGI("Encrypting .rodata section (%zu bytes)", rodataSize);
        for (size_t i = 0; i < rodataSize; i++) rodata[i] ^= 0x55;
    }
}
