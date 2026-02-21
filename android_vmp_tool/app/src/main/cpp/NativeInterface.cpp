#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include <android/log.h>
#include <vector>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "VMP_Native", __VA_ARGS__)

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_MainActivity_protectAndPackSo(JNIEnv* env, jobject thiz, jstring input_path, jstring output_path, jstring lib_dir) {
    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);
    const char* l_dir = env->GetStringUTFChars(lib_dir, NULL);

    jclass clazz = env->GetObjectClass(thiz);
    jmethodID logMethod = env->GetMethodID(clazz, "onLog", "(Ljava/lang/String;)V");
    auto logger = [&](const std::string& msg) {
        jstring jmsg = env->NewStringUTF(msg.c_str());
        env->CallVoidMethod(thiz, logMethod, jmsg);
        env->DeleteLocalRef(jmsg);
    };

    bool finalSuccess = false;
    std::string loaderPath;

    logger("[INIT] ANALYZING TARGET ELF...");
    {
        ElfParser target(in_path);
        if (!target.parse()) {
            logger("[ERROR] FAILED TO PARSE TARGET.");
            goto cleanup;
        }

        logger("[VMP] ENCRYPTING PAYLOAD...");
        for (size_t i = 0; i < target.mSize; i++) target.mData[i] ^= 0xAA;

        loaderPath = std::string(l_dir) + "/libvmp_loader.so";
        logger("[STUB] ACQUIRING LOADER: " + loaderPath);

        ElfParser loader(loaderPath.c_str());
        if (!loader.parse()) {
            logger("[ERROR] LOADER STUB NOT ACCESSIBLE.");
            goto cleanup;
        }

        if (loader.is64Bit != target.is64Bit) {
            logger("[ERROR] ARCHITECTURE MISMATCH.");
            goto cleanup;
        }

        logger("[PACK] INJECTING PAYLOAD INTO STUB...");
        if (!loader.patchSection(".vmp_payload", target.mData, target.mSize)) {
            logger("[ERROR] PAYLOAD EXCEEDS STUB CAPACITY (15MB).");
            goto cleanup;
        }

        if (!loader.patchSection(".vmp_info", (uint8_t*)&target.mSize, sizeof(size_t))) {
            logger("[ERROR] STUB METADATA UPDATE FAILED.");
            goto cleanup;
        }

        if (loader.save(out_path)) {
            logger("[SUCCESS] PACKED SO GENERATED.");
            finalSuccess = true;
        }
    }

cleanup:
    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    env->ReleaseStringUTFChars(lib_dir, l_dir);
    return finalSuccess ? JNI_TRUE : JNI_FALSE;
}
