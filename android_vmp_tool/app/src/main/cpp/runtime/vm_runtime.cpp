#include "vm_runtime.h"
#include <stdio.h>
#include <android/log.h>

#define LOG_TAG "VMP_VM"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

void vmp_execute(const uint8_t* bytecode, void* context) {
    LOGI("VM executing bytecode...");
    const uint8_t* pc = bytecode;
    while (*pc != 0xFF) { // 0xFF as EXIT opcode
        uint8_t opcode = *pc++;
        switch (opcode) {
            case 0x01: // Dummy ADD
                LOGI("VM: Executing ADD");
                break;
            case 0x02: // Dummy JMP
                LOGI("VM: Executing JMP");
                break;
            default:
                LOGI("VM: Unknown opcode %02x", opcode);
                return;
        }
    }
}
