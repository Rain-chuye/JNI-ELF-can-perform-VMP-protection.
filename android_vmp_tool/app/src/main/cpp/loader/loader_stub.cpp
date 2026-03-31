#include <jni.h>
#include <android/log.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h>
#include <fcntl.h>
#include <string>
#include <elf.h>
#include "linker.h"

#define LOG_TAG "VMP_Loader"
#define MAGIC 0x564D50534849454CLL

#ifdef __arm__
#define Elf_Ehdr Elf32_Ehdr
#else
#define Elf_Ehdr Elf64_Ehdr
#endif

struct Footer {
    size_t metadata_size;
    size_t payload_size;
    unsigned long long magic;
};

void* load_payload(const char* self_path) {
    int fd = open(self_path, O_RDONLY);
    if (fd < 0) return NULL;

    off_t file_size = lseek(fd, 0, SEEK_END);
    lseek(fd, file_size - sizeof(Footer), SEEK_SET);
    Footer footer;
    read(fd, &footer, sizeof(Footer));

    if (footer.magic != MAGIC) {
        close(fd);
        return NULL;
    }

    lseek(fd, file_size - sizeof(Footer) - footer.metadata_size - footer.payload_size, SEEK_SET);
    unsigned char* payload = (unsigned char*)malloc(footer.payload_size);
    read(fd, payload, footer.payload_size);
    close(fd);

    for (size_t i = 0; i < footer.payload_size; i++) payload[i] ^= 0xAA;

    void* handle = vmp_load_library_from_mem(payload, footer.payload_size);
    free(payload);
    return handle;
}

extern "C" JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* reserved) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Shield V14: Initializing...");
    Dl_info info;
    if (dladdr((void*)JNI_OnLoad, &info) == 0) return JNI_ERR;
    void* handle = load_payload(info.dli_fname);
    if (!handle) return JNI_VERSION_1_6;
    typedef jint (*jni_onload_t)(JavaVM*, void*);
    jni_onload_t real_onload = (jni_onload_t)vmp_find_symbol(handle, "JNI_OnLoad");
    return real_onload ? real_onload(vm, reserved) : JNI_VERSION_1_6;
}

extern "C" int main(int argc, char** argv, char** envp) {
    __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "Shield Executive Active.");
    void* handle = load_payload("/proc/self/exe");
    if (!handle) return 1;
    Elf_Ehdr* ehdr = (Elf_Ehdr*)handle;
    typedef int (*entry_t)(int, char**, char**);
    entry_t entry = (entry_t)((uintptr_t)handle + ehdr->e_entry);
    return entry(argc, argv, envp);
}
