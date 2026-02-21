#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include <android/log.h>
#include <vector>
#include <fstream>
#include <sstream>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "VMP_Native", __VA_ARGS__)
#define MAGIC 0x564D50534849454CLL

struct Footer {
    size_t metadata_size;
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
        ElfParser target(in_path);
        if (!target.parse()) {
            logger("[ERROR] FAILED TO PARSE TARGET.");
            goto cleanup;
        }

        // Extract JNI metadata
        std::vector<JniExport> exports = target.getJniExports();
        logger("[META] FOUND " + std::to_string(exports.size()) + " JNI EXPORTS.");

        std::stringstream meta_ss;
        uint32_t export_count = exports.size();
        meta_ss.write((char*)&export_count, 4);
        for (const auto& ex : exports) {
            uint32_t name_len = ex.name.length();
            meta_ss.write((char*)&name_len, 4);
            meta_ss.write(ex.name.c_str(), name_len);
            uint64_t offset = (uint64_t)ex.offset;
            meta_ss.write((char*)&offset, 8);
        }
        std::string metadata = meta_ss.str();

        // 1. Encrypt target
        logger("[VMP] ENCRYPTING PAYLOAD...");
        for (size_t i = 0; i < target.mSize; i++) target.mData[i] ^= 0xAA;

        // 2. Select Loader Stub
        std::string loaderPath = std::string(l_dir) + "/libvmp_loader.so";
        std::ifstream stub_in(loaderPath, std::ios::binary);
        if (!stub_in) {
            logger("[ERROR] LOADER STUB NOT FOUND.");
            goto cleanup;
        }

        std::ofstream out_file(out_path, std::ios::binary);
        out_file << stub_in.rdbuf();
        stub_in.close();

        // 3. Append encrypted payload
        out_file.write((char*)target.mData, target.mSize);

        // 4. Append metadata
        out_file.write(metadata.data(), metadata.size());

        // 5. Append Footer
        Footer footer = { metadata.size(), target.mSize, MAGIC };
        out_file.write((char*)&footer, sizeof(Footer));
        out_file.close();

        logger("[SUCCESS] VMP STABILITY-ENHANCED SO GENERATED.");
        finalSuccess = true;
    }

cleanup:
    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    env->ReleaseStringUTFChars(lib_dir, l_dir);
    return finalSuccess ? JNI_TRUE : JNI_FALSE;
}
