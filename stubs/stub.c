// Pure C stub without headers
typedef unsigned long size_t;

long syscall(long number, ...);

void decrypt_stub(void* base, size_t offset, size_t size, unsigned char key) {
    unsigned char* start = (unsigned char*)base + offset;
    unsigned long page_start = (unsigned long)start & ~4095UL;
    unsigned long page_end = ((unsigned long)start + size + 4095UL) & ~4095UL;
    size_t page_len = page_end - page_start;

    // mprotect syscall numbers: ARM64=226, ARM32=125
#if defined(__aarch64__)
    long nr_mprotect = 226;
#else
    long nr_mprotect = 125;
#endif

    // We can't easily call syscall() without libc.
    // We'll use inline assembly instead.
}
