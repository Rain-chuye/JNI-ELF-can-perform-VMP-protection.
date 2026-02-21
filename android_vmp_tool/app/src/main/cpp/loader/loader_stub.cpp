#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include "linker.h"

#define LOG_TAG "VMP_Loader"

// Pre-allocated space for payload (15MB)
#define MAX_PAYLOAD 15 * 1024 * 1024
__attribute__((section(".vmp_payload"), used)) unsigned char vmp_payload[MAX_PAYLOAD] = {0xDE, 0xAD, 0xBE, 0xEF};
__attribute__((section(".vmp_info"), used)) size_t vmp_payload_size = 0;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Secure Loader Active.");

    if (vmp_payload_size == 0 || vmp_payload_size > MAX_PAYLOAD) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Invalid payload size: %zu", vmp_payload_size);
        return JNI_VERSION_1_6;
    }

    unsigned char* decrypted = (unsigned char*)malloc(vmp_payload_size);
    for (size_t i = 0; i < vmp_payload_size; i++) {
        decrypted[i] = vmp_payload[i] ^ 0xAA;
    }

    void* handle = vmp_load_library_from_mem(decrypted, vmp_payload_size);
    if (!handle) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Critical: Linker failed to map encrypted payload.");
        free(decrypted);
        return JNI_ERR;
    }

    // Hand over to the original JNI_OnLoad
    typedef jint (*jni_onload_t)(JavaVM*, void*);
    jni_onload_t real_onload = (jni_onload_t)vmp_find_symbol(handle, "JNI_OnLoad");

    if (real_onload) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Handing over to original JNI_OnLoad.");
        return real_onload(vm, reserved);
    }

    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Payload loaded. No JNI_OnLoad found.");
    return JNI_VERSION_1_6;
}
