void stub(void* base, unsigned long offset, unsigned long size, unsigned char key) {
    unsigned char* p = (unsigned char*)base + offset;
    for (unsigned long i = 0; i < size; i++) p[i] ^= key;
}
