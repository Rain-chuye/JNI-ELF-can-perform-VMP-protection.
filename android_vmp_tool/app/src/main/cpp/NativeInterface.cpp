#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include "engine/string_obfuscator.h"
#include "engine/jni_protector.h"
#include <android/log.h>
#include <sys/stat.h>

#define LOG_TAG "VMP_Tool"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_MainActivity_protectSo(JNIEnv* env, jobject thiz, jstring input_path, jstring output_path) {
    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);

    LOGI("Protecting %s -> %s", in_path, out_path);

    struct stat st;
    if (stat(in_path, &st) != 0) {
        LOGE("Input file does not exist or not accessible: %s", in_path);
        env->ReleaseStringUTFChars(input_path, in_path);
        env->ReleaseStringUTFChars(output_path, out_path);
        return JNI_FALSE;
    }
    LOGI("Input file size: %ld", (long)st.st_size);

    ElfParser parser(in_path);
    if (!parser.parse()) {
        LOGE("Failed to parse ELF file: %s", in_path);
        env->ReleaseStringUTFChars(input_path, in_path);
        env->ReleaseStringUTFChars(output_path, out_path);
        return JNI_FALSE;
    }

    VmpEngine vmp(&parser);
    vmp.protect();

    StringObfuscator so(&parser);
    so.obfuscate();

    JniProtector jp(&parser);
    jp.protect();

    bool success = parser.save(out_path);
    if (!success) {
        LOGE("Failed to save protected ELF to: %s", out_path);
    } else {
        LOGI("Successfully protected and saved to: %s", out_path);
    }

    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);

    return success ? JNI_TRUE : JNI_FALSE;
}
