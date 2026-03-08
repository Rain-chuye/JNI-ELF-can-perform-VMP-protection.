#include <stdio.h>
#include <string.h>

// Simulated JNI Export
const char* Java_com_vmp_shield_NativeLib_getSecret(void* env, void* obj) {
    return "This is a JNI-embedded secret key: VMP-12345678";
}

// Internal sensitive function
static void internal_verify_license() {
    printf("Internal license verification passed.\n");
}

void public_api() {
    internal_verify_license();
    printf("Public API called.\n");
}
