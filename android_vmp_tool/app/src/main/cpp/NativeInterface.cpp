#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include <android/log.h>
#include <sys/stat.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "VMP_Native", __VA_ARGS__)

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_MainActivity_protectSo(JNIEnv* env, jobject thiz, jstring input_path, jstring output_path) {
    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID logMethod = env->GetMethodID(clazz, "onLog", "(Ljava/lang/String;)V");
    auto logger = [&](const std::string& msg) {
        jstring jmsg = env->NewStringUTF(msg.c_str());
        env->CallVoidMethod(thiz, logMethod, jmsg);
        env->DeleteLocalRef(jmsg);
    };

    LOGI("Processing: %s", in_path);
    logger("[TASK] LOADING TARGET: " + std::string(in_path));

    ElfParser parser(in_path);
    if (!parser.parse()) {
        logger("[ERROR] INVALID ELF FILE.");
        env->ReleaseStringUTFChars(input_path, in_path);
        env->ReleaseStringUTFChars(output_path, out_path);
        return JNI_FALSE;
    }

    // 1. Internal Protection (VMP/Obfuscation)
    VmpEngine engine(&parser);
    engine.setLogCallback(logger);
    engine.protect();

    // 2. Wrap in Loader (This logic would ideally find the stub SO and patch it)
    // For this POC, we'll save the encrypted SO.
    // In the commercial version, we'd output the patched libvmp_loader.so

    bool success = parser.save(out_path);
    logger("[COMPLETED] PROTECTED FILE SAVED.");

    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    return success ? JNI_TRUE : JNI_FALSE;
}
