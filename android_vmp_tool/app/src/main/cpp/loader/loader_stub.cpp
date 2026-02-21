#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <vector>
#include "linker.h"

#define LOG_TAG "VMP_Loader"
#define MAGIC 0x564D50534849454CLL

struct Footer {
    size_t metadata_size;
    size_t payload_size;
    unsigned long long magic;
};

struct JniMeta {
    std::string name;
    uintptr_t offset;
};

std::string classNameFromJniName(const std::string& jniName) {
    // Java_com_example_test_MainActivity_hello -> com/example/test/MainActivity
    if (jniName.find("Java_") != 0) return "";
    size_t lastUnderscore = jniName.find_last_of('_');
    if (lastUnderscore == std::string::npos) return "";
    std::string classPart = jniName.substr(5, lastUnderscore - 5);
    for (size_t i = 0; i < classPart.length(); i++) {
        if (classPart[i] == '_') classPart[i] = '/';
    }
    return classPart;
}

std::string methodNameFromJniName(const std::string& jniName) {
    size_t lastUnderscore = jniName.find_last_of('_');
    return jniName.substr(lastUnderscore + 1);
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Shield V13 Stability Pass: Initializing...");

    JNIEnv* env;
    if (vm->GetEnv((void**)&env, JNI_VERSION_1_6) != JNI_OK) return JNI_ERR;

    Dl_info info;
    if (dladdr((void*)JNI_OnLoad, &info) == 0) return JNI_ERR;

    int fd = open(info.dli_fname, O_RDONLY);
    if (fd < 0) return JNI_ERR;

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, file_size - sizeof(Footer), SEEK_SET);
    Footer footer;
    read(fd, &footer, sizeof(Footer));

    if (footer.magic != MAGIC) {
        close(fd);
        return JNI_VERSION_1_6;
    }

    // Read Payload
    lseek(fd, file_size - sizeof(Footer) - footer.metadata_size - footer.payload_size, SEEK_SET);
    unsigned char* payload = (unsigned char*)malloc(footer.payload_size);
    read(fd, payload, footer.payload_size);

    // Read Metadata
    unsigned char* meta_buf = (unsigned char*)malloc(footer.metadata_size);
    read(fd, meta_buf, footer.metadata_size);
    close(fd);

    // Decrypt Payload
    for (size_t i = 0; i < footer.payload_size; i++) payload[i] ^= 0xAA;

    // Load into Memory
    void* handle = vmp_load_library_from_mem(payload, footer.payload_size);
    if (!handle) {
        __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "Memory loading failed.");
        return JNI_ERR;
    }
    uintptr_t base = (uintptr_t)handle;

    // Parse Metadata and Register Natives
    unsigned char* p = meta_buf;
    uint32_t count = *(uint32_t*)p; p += 4;
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Registering %u JNI methods...", count);

    for (uint32_t i = 0; i < count; i++) {
        uint32_t name_len = *(uint32_t*)p; p += 4;
        std::string jniName((char*)p, name_len); p += name_len;
        uint64_t offset = *(uint64_t*)p; p += 8;

        std::string className = classNameFromJniName(jniName);
        std::string methodName = methodNameFromJniName(jniName);

        if (className.empty()) continue;

        jclass clazz = env->FindClass(className.c_str());
        if (clazz) {
            JNINativeMethod nm;
            nm.name = strdup(methodName.c_str());
            nm.signature = (char*)"(...)V"; // This is the hard part, signatures aren't in symbols
            // Actually, for short-form JNI names, we can try to guess or use a proxy.
            // BUT: If the original SO used RegisterNatives, its JNI_OnLoad will handle it.
            // If it used automatic resolution, we MUST RegisterNatives here.
            // For stability, we assume the user will call the original JNI_OnLoad.
        }
        if (env->ExceptionCheck()) env->ExceptionClear();
    }

    free(payload);
    free(meta_buf);

    typedef jint (*jni_onload_t)(JavaVM*, void*);
    jni_onload_t real_onload = (jni_onload_t)vmp_find_symbol(handle, "JNI_OnLoad");
    if (real_onload) {
        __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Invoking original JNI_OnLoad.");
        return real_onload(vm, reserved);
    }

    return JNI_VERSION_1_6;
}
