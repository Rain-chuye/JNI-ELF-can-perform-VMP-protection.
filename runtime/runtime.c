#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <ctype.h>
#include <signal.h>

void decrypt_data(char* data, size_t len, char key) {
    if (!data) return;
    for (size_t i = 0; i < len; ++i) {
        data[i] ^= key;
    }
}

// Simple CRC32 for integrity check
uint32_t calculate_crc32(const char* data, size_t len) {
    uint32_t crc = 0xFFFFFFFF;
    for (size_t i = 0; i < len; i++) {
        crc ^= (uint8_t)data[i];
        for (int j = 0; j < 8; j++) {
            crc = (crc >> 1) ^ (0xEDB88320 & (-(crc & 1)));
        }
    }
    return ~crc;
}

void anti_debug_init(void) {
#ifdef __linux__
    // Signal-based anti-debug: Trap if being debugged
    if (ptrace(PTRACE_TRACEME, 0, 1, 0) == 0) {
        ptrace(PTRACE_DETACH, 0, 1, 0);
    } else if (errno == EPERM) {
        // Someone else is already tracing us
        exit(1);
    }

    // Check for common debuggers in memory/proc
    if (access("/usr/bin/gdb", F_OK) == 0) {
        // Just a hint, not an exit
    }
#endif
}

void vm_interpreter(const unsigned char* bytecode, void* args[]) {
    // Advanced VM dispatcher could go here
}

__attribute__((constructor))
void vmp_init() {
    anti_debug_init();
}
