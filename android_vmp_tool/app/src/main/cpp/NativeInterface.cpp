#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include <android/log.h>
#include <vector>

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_MainActivity_protectSo(JNIEnv* env, jobject thiz, jstring input_path, jstring output_path) {
    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);

    __android_log_print(ANDROID_LOG_INFO, "VMP_Tool", "Robust protection for: %s", in_path);

    ElfParser parser(in_path);
    if (!parser.parse()) {
        env->ReleaseStringUTFChars(input_path, in_path);
        env->ReleaseStringUTFChars(output_path, out_path);
        return JNI_FALSE;
    }

    // SAFE PROTECTION STRATEGY:
    // 1. Encrypt .rodata (Strings)
    size_t roSize = 0;
    uint8_t* rodata = parser.getSection(".rodata", &roSize);
    if (rodata) {
        for (size_t i = 0; i < roSize; i++) rodata[i] ^= 0x42;
    }

    // 2. Encrypt JNI Export bodies (avoiding the prologue)
    size_t dynsymSize;
    uint8_t* dynsym = parser.getSection(".dynsym", &dynsymSize);
    uint8_t* dynstr = parser.getSection(".dynstr", NULL);
    if (dynsym && dynstr) {
        if (parser.is64Bit) {
            Elf64_Sym* syms = (Elf64_Sym*)dynsym;
            for (int i = 0; i < dynsymSize / sizeof(Elf64_Sym); i++) {
                const char* name = (const char*)dynstr + syms[i].st_name;
                if (strncmp(name, "Java_", 5) == 0 && syms[i].st_size > 16) {
                    // Skip the first 16 bytes (prologue) to avoid breaking basic stack frames
                    uint8_t* func = parser.mData + syms[i].st_value;
                    for (size_t j = 16; j < syms[i].st_size; j++) func[j] ^= 0x77;
                }
            }
        }
    }

    bool success = parser.save(out_path);

    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    return success ? JNI_TRUE : JNI_FALSE;
}
