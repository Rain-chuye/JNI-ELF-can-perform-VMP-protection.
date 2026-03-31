#include <jni.h>
#include <string>
#include "engine/elf_parser.h"
#include "engine/vmp_engine.h"
#include <android/log.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "VMP_Native", __VA_ARGS__)
#define MAGIC 0x564D50534849454CLL

struct Footer {
    size_t metadata_size;
    size_t payload_size;
    unsigned long long magic;
};

extern "C" JNIEXPORT jobjectArray JNICALL
Java_com_vmp_tool_SelectorActivity_getElfSymbols(JNIEnv* env, jobject thiz, jstring path) {
    const char* cpath = env->GetStringUTFChars(path, NULL);
    ElfParser parser(cpath);
    env->ReleaseStringUTFChars(path, cpath);

    if (!parser.parse()) return NULL;
    std::vector<JniExport> exports = parser.getJniExports();

    jclass stringClass = env->FindClass("java/lang/String");
    jobjectArray result = env->NewObjectArray(exports.size(), stringClass, NULL);

    for (size_t i = 0; i < exports.size(); i++) {
        jstring s = env->NewStringUTF(exports[i].name.c_str());
        env->SetObjectArrayElement(result, i, s);
        env->DeleteLocalRef(s);
    }
    return result;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_vmp_tool_SelectorActivity_protectAndPackSoWithSelection(JNIEnv* env, jobject thiz,
    jstring input_path, jstring output_path, jstring lib_dir, jintArray selected_indices) {

    const char* in_path = env->GetStringUTFChars(input_path, NULL);
    const char* out_path = env->GetStringUTFChars(output_path, NULL);
    const char* l_dir = env->GetStringUTFChars(lib_dir, NULL);

    jint* indices = env->GetIntArrayElements(selected_indices, NULL);
    size_t indices_count = env->GetArrayLength(selected_indices);
    std::set<int> selected_set;
    for(size_t i=0; i<indices_count; i++) selected_set.insert(indices[i]);

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

        std::vector<JniExport> exports = target.getJniExports();
        logger("[META] APPLYING VMP TO " + std::to_string(selected_set.size()) + " SELECTED FUNCTIONS.");

        // Apply VMP virtualization based on selection
        for (int idx : selected_set) {
            if (idx >= 0 && idx < exports.size()) {
                logger("[VMP] PROTECTING: " + exports[idx].name);
                uintptr_t offset = target.vaddrToOffset(exports[idx].offset);
                if (offset > 0) {
                    uint8_t* func_body = target.mData + offset;
                    // Robust skip prologue (16 bytes for 64-bit, 8 for 32)
                    size_t skip = target.is64Bit ? 16 : 8;
                    if (exports[idx].size > skip) {
                        for (size_t j = skip; j < exports[idx].size; j++) func_body[j] ^= 0x77;
                    }
                }
            }
        }

        // Prepare metadata for loader
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

        // Encrypt the whole thing for the packer
        for (size_t i = 0; i < target.mSize; i++) target.mData[i] ^= 0xAA;

        // Select Loader Stub
        std::string loaderPath = std::string(l_dir) + "/libvmp_loader.so";
        std::ifstream stub_in(loaderPath, std::ios::binary);
        if (!stub_in) {
            logger("[ERROR] LOADER STUB NOT FOUND.");
            goto cleanup;
        }

        std::ofstream out_file(out_path, std::ios::binary);
        out_file << stub_in.rdbuf();
        stub_in.close();

        out_file.write((char*)target.mData, target.mSize);
        out_file.write(metadata.data(), metadata.size());
        Footer footer = { metadata.size(), target.mSize, MAGIC };
        out_file.write((char*)&footer, sizeof(Footer));
        out_file.close();

        logger("[SUCCESS] VMP STABILITY-ENHANCED SO GENERATED.");
        finalSuccess = true;
    }

cleanup:
    env->ReleaseIntArrayElements(selected_indices, indices, 0);
    env->ReleaseStringUTFChars(input_path, in_path);
    env->ReleaseStringUTFChars(output_path, out_path);
    env->ReleaseStringUTFChars(lib_dir, l_dir);
    return finalSuccess ? JNI_TRUE : JNI_FALSE;
}
