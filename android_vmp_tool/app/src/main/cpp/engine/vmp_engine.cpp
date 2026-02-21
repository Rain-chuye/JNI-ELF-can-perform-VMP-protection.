#include "vmp_engine.h"
#include <android/log.h>
#include <string.h>
#include <vector>

#define LOG_TAG "VMP_Engine"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

VmpEngine::VmpEngine(ElfParser* parser) : mParser(parser) {}

void VmpEngine::log(const std::string& msg) {
    if (mLogCallback) mLogCallback(msg);
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "%s", msg.c_str());
}

void VmpEngine::protect() {
    log("[SYSTEM] ROOTLESS PROTECTION MODE INITIATED");

    // In this mode, we encrypt the entire SO and prepare it for the loader stub.
    // The "protect" call here is actually the "encryption" phase.

    log("[CORE] ENCRYPTING ENTIRE BINARY PAYLOAD...");
    for (size_t i = 0; i < mParser->mSize; i++) {
        mParser->mData[i] ^= 0xAA; // Match loader_stub.cpp key
    }

    log("[SUCCESS] BINARY ENCRYPTED. READY FOR STUB INJECTION.");
}
