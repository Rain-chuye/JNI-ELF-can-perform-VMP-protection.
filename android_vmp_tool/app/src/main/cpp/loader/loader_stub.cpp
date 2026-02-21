#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include "linker.h"

#define LOG_TAG "VMP_Loader"
#define MAGIC 0x564D50534849454CLL

struct Footer {
    size_t payload_size;
    unsigned long long magic;
};

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
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

    lseek(fd, file_size - sizeof(Footer) - footer.payload_size, SEEK_SET);
    unsigned char* encrypted = (unsigned char*)malloc(footer.payload_size);
    read(fd, encrypted, footer.payload_size);
    close(fd);

    for (size_t i = 0; i < footer.payload_size; i++) encrypted[i] ^= 0xAA;

    void* handle = vmp_load_library_from_mem(encrypted, footer.payload_size);
    if (!handle) {
        free(encrypted);
        return JNI_ERR;
    }

    typedef jint (*jni_onload_t)(JavaVM*, void*);
    jni_onload_t real_onload = (jni_onload_t)vmp_find_symbol(handle, "JNI_OnLoad");
    return real_onload ? real_onload(vm, reserved) : JNI_VERSION_1_6;
}
