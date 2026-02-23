#include <stdio.h>
#include <string.h>

void secret_logic(int a, int b) {
    char secret[] = "This is a very secret message!";
    int result = 0;
    if (a > b) {
        result = a * 2 + b;
        printf("Branch A: %d\n", result);
    } else {
        result = b * 3 - a;
        printf("Branch B: %d\n", result);
    }

    for (int i = 0; i < 5; i++) {
        result += i;
    }
    printf("Final result: %d\n", result);
    printf("Secret: %s\n", secret);
}

int main() {
    printf("Starting complex test...\n");
    secret_logic(10, 5);
    secret_logic(3, 7);
    return 0;
}
