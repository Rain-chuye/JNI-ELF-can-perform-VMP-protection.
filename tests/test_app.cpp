#include <iostream>
#include <string>

// Marker for VMP
#define VMP_PROTECT __attribute__((annotate("vmp")))

VMP_PROTECT void secret_logic() {
    std::string secret = "This is a super secret string!";
    std::cout << "Running secret logic..." << std::endl;
    std::cout << "Secret: " << secret << std::endl;
}

int main() {
    std::cout << "Hello from unprotected part!" << std::endl;
    secret_logic();
    return 0;
}
