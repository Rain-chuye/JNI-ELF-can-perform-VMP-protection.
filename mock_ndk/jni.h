struct JNIEnvStruct {
    const char* GetStringUTFChars(void* s, bool* b) { return ""; }
    void ReleaseStringUTFChars(void* s, const char* c) {}
};
typedef JNIEnvStruct* JNIEnv;
typedef void* jobject;
typedef void* jstring;
typedef int jboolean;
#define JNIEXPORT
#define JNICALL
#define JNI_TRUE 1
#define JNI_FALSE 0
