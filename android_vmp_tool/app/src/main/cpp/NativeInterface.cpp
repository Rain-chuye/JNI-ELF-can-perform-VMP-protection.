#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include <android/log.h>
#include <vector>
#include <fstream>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "VMP_Native", __VA_ARGS__)
#define MAGIC 0x564D50534849454CLL

struct Footer {
    size_t payload_size;
    unsigned long long magic;
};

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

    logger("[INIT] ANALYZING TARGET...");
    {
        std::ifstream in_file(in_path, std::ios::binary);
        if (!in_file) {
            logger("[ERROR] CANNOT OPEN INPUT.");
            goto cleanup;
        }

        // Detect Arch
        unsigned char ident[16];
        in_file.read((char*)ident, 16);
        bool is64 = (ident[4] == 2);
        in_file.seekg(0, std::ios::end);
        size_t in_size = in_file.tellg();
        in_file.seekg(0, std::ios::beg);

        std::vector<unsigned char> buffer(in_size);
        in_file.read((char*)buffer.data(), in_size);
        in_file.close();

        logger(std::string("[ARCH] DETECTED ") + (is64 ? "ARM64" : "ARM32"));

        // 1. Encrypt
        for (size_t i = 0; i < in_size; i++) buffer[i] ^= 0xAA;

        // 2. Select Loader Stub (Requires that the app assets/libs have both)
        // Since Gradle builds all ABIs, we find the right one in the app's lib dir
        std::string stubName = is64 ? "arm64-v8a" : "armeabi-v7a";
        // We need to look in the right subfolder of lib_dir
        // Actually, Android installs only the matching ABI's libs.
        // We must ensure the APK contains BOTH.
        std::string loaderPath = std::string(l_dir) + "/libvmp_loader.so";
        // NOTE: In a real app, you'd distribute the stubs separately or
        // put them in assets to ensure both are always available.

        logger("[STUB] READING LOADER CORE...");
        std::ifstream stub_in(loaderPath, std::ios::binary);
        if (!stub_in) {
            // Fallback: try to guess the path if on 64-bit phone but target is 32
            // This is a complex part of cross-arch tool design.
            logger("[ERROR] LOADER STUB NOT FOUND FOR ARCH.");
            goto cleanup;
        }

        std::ofstream out_file(out_path, std::ios::binary);
        out_file << stub_in.rdbuf(); // Copy loader stub
        stub_in.close();

        // 3. Append encrypted payload
        logger("[PACK] APPENDING PAYLOAD...");
        out_file.write((char*)buffer.data(), in_size);

        // 4. Append Footer
        Footer footer = { in_size, MAGIC };
        out_file.write((char*)&footer, sizeof(Footer));
        out_file.close();

        logger("[SUCCESS] VMP ULTIMATE PACKED FILE GENERATED.");
        finalSuccess = true;
    }

cleanup:
    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    env->ReleaseStringUTFChars(lib_dir, l_dir);
    return finalSuccess ? JNI_TRUE : JNI_FALSE;
}
