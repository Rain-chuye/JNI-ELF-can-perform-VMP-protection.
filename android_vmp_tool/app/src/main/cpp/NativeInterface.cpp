#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include <android/log.h>

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_MainActivity_protectSo(JNIEnv* env, jobject thiz, jstring input_path, jstring output_path) {
    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);

    __android_log_print(ANDROID_LOG_INFO, "VMP_Tool", "Stability Protection for: %s", in_path);

    ElfParser parser(in_path);
    if (!parser.parse()) {
        env->ReleaseStringUTFChars(input_path, in_path);
        env->ReleaseStringUTFChars(output_path, out_path);
        return JNI_FALSE;
    }

    VmpEngine engine(&parser);
    engine.protect();

    bool success = parser.save(out_path);

    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    return success ? JNI_TRUE : JNI_FALSE;
}
