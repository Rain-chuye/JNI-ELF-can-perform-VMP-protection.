#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include "engine/string_obfuscator.h"
#include "engine/jni_protector.h"
#include <android/log.h>

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_MainActivity_protectSo(JNIEnv* env, jobject thiz, jstring input_path, jstring output_path) {
    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);

    __android_log_print(ANDROID_LOG_INFO, "VMP_Tool", "Protecting %s -> %s", in_path, out_path);

    ElfParser parser(in_path);
    if (!parser.parse()) {
        __android_log_print(ANDROID_LOG_ERROR, "VMP_Tool", "Failed to parse ELF");
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

    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);

    return success ? JNI_TRUE : JNI_FALSE;
}
