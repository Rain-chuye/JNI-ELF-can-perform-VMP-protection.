#include <jni.h>
#include <string>
#include <android/log.h>

extern "C" JNIEXPORT jstring JNICALL
Java_com_example_test_MainActivity_stringFromJNI(JNIEnv* env, jobject thiz) {
    std::string hello = "Hello from Protected JNI! This string should be hidden.";
    __android_log_print(ANDROID_LOG_INFO, "TestLib", "Executing protected JNI function");
    return env->NewStringUTF(hello.c_str());
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_test_MainActivity_secretLogic(JNIEnv* env, jobject thiz) {
    int a = 10, b = 20;
    int sum = a + b;
    __android_log_print(ANDROID_LOG_INFO, "TestLib", "Secret logic result: %d", sum);
}
