#include <stdint.h>
#include <stddef.h>

void vmp_decrypt(uint8_t* data, size_t len, uint8_t key) {
    for (size_t i = 0; i < len; i++) {
        data[i] ^= key;
    }
}
