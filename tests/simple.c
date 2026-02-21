#include <stdio.h>

void secret_func() {
    printf("Secret: %s\n", "ThisIsASecretString");
}

int main() {
    secret_func();
    return 0;
}
