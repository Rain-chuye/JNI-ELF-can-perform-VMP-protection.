#include <jni.h>
#include <string>
#include <iostream>
#include <fstream>
#include <vector>

// Simulating the protection logic inside JNI
extern "C" JNIEXPORT jint JNICALL
Java_com_vmp_shield_MainActivity_protectFileNative(
        JNIEnv* env,
        jobject /* this */,
        jstring inputPath,
        jstring outputPath) {

    const char* nativeInput = env->GetStringUTFChars(inputPath, 0);
    const char* nativeOutput = env->GetStringUTFChars(outputPath, 0);

    // Simple XOR protection demo (re-using elf_packer logic)
    std::ifstream in(nativeInput, std::ios::binary);
    if (!in) {
        env->ReleaseStringUTFChars(inputPath, nativeInput);
        env->ReleaseStringUTFChars(outputPath, nativeOutput);
        return -1;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    // Encrypt logic
    for (size_t i = 0; i < buffer.size(); i++) {
        buffer[i] ^= 0xAA;
    }

    std::ofstream out(nativeOutput, std::ios::binary);
    out.write(buffer.data(), buffer.size());
    out.close();

    env->ReleaseStringUTFChars(inputPath, nativeInput);
    env->ReleaseStringUTFChars(outputPath, nativeOutput);

    return 0;
}
