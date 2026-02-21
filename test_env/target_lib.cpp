#include <stdio.h>

extern "C" void hello() {
    printf("Hello from the target library!\n");
}

extern "C" int add(int a, int b) {
    return a + b;
}
