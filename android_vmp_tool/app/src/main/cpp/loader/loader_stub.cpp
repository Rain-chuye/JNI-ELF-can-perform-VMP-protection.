#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include "linker.h"

#define LOG_TAG "VMP_Loader"

// Pre-allocated space for payload
#define MAX_PAYLOAD 10 * 1024 * 1024
unsigned char vmp_payload[MAX_PAYLOAD] __attribute__((section(".vmp_payload"))) = {0xDE, 0xAD, 0xBE, 0xEF};
size_t vmp_payload_size __attribute__((section(".vmp_info"))) = 0;

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Shield active. Decrypting payload...");

    if (vmp_payload_size == 0) {
        // Fallback for demo: if size is 0, we can't load
        return JNI_VERSION_1_6;
    }

    unsigned char* decrypted = (unsigned char*)malloc(vmp_payload_size);
    for (size_t i = 0; i < vmp_payload_size; i++) {
        decrypted[i] = vmp_payload[i] ^ 0xAA;
    }

    void* handle = vmp_load_library_from_mem(decrypted, vmp_payload_size);
    if (!handle) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Linker failure.");
        free(decrypted);
        return JNI_ERR;
    }

    typedef jint (*jni_onload_t)(JavaVM*, void*);
    jni_onload_t real_onload = (jni_onload_t)vmp_find_symbol(handle, "JNI_OnLoad");

    if (real_onload) {
        return real_onload(vm, reserved);
    }

    return JNI_VERSION_1_6;
}
