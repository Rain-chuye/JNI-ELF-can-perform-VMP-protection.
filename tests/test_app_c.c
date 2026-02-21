#include <stdio.h>

void secret_logic() {
    char* secret = "This is a super secret string!";
    printf("Running secret logic...\n");
    printf("Secret: %s\n", secret);
}

int main() {
    printf("Hello from unprotected part!\n");
    secret_logic();
    return 0;
}
