#include "security.h"
#include <sys/ptrace.h>
#include <unistd.h>
#include <stdlib.h>
#include <android/log.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "VMP_SECURITY", __VA_ARGS__)

void vmp_anti_debug() {
    // Simple ptrace anti-debug
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) < 0) {
        LOGI("Debugger detected! Terminating process...");
        exit(1);
    }
}

void vmp_security_check() {
    vmp_anti_debug();
    // Additional checks can be added here
}

// Global constructor to run before main
__attribute__((constructor))
void vmp_init_security() {
    vmp_security_check();
}
