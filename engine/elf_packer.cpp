#include <iostream>
#include <fstream>
#include <vector>
#include <elf.h>
#include <sys/mman.h>
#include <cstring>

void encrypt_section(const char* filename, const char* output_filename) {
    std::ifstream in(filename, std::ios::binary);
    if (!in) {
        std::cerr << "Cannot open file: " << filename << std::endl;
        return;
    }

    std::vector<char> buffer((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
    in.close();

    Elf64_Ehdr* ehdr = (Elf64_Ehdr*)buffer.data();
    if (memcmp(ehdr->e_ident, ELFMAG, SELFMAG) != 0) {
        std::cerr << "Not a valid ELF file" << std::endl;
        return;
    }

    // Find the .text section
    Elf64_Shdr* shdr = (Elf64_Shdr*)(buffer.data() + ehdr->e_shoff);
    const char* shstrtab = buffer.data() + shdr[ehdr->e_shstrndx].sh_offset;

    for (int i = 0; i < ehdr->e_shnum; i++) {
        const char* name = shstrtab + shdr[i].sh_name;
        if (strcmp(name, ".text") == 0) {
            std::cout << "[*] Found .text section at offset 0x" << std::hex << shdr[i].sh_offset
                      << " size 0x" << shdr[i].sh_size << std::endl;

            // Encrypt it
            for (size_t j = 0; j < shdr[i].sh_size; j++) {
                buffer[shdr[i].sh_offset + j] ^= 0xAA;
            }
            std::cout << "[+] Encrypted .text section" << std::endl;
            break;
        }
    }

    // In a real packer, we would also:
    // 1. Add a decryption stub (init_array entry)
    // 2. Add anti-debug logic

    std::ofstream out(output_filename, std::ios::binary);
    out.write(buffer.data(), buffer.size());
    out.close();
    std::cout << "[+] Saved packed binary to: " << output_filename << std::endl;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        std::cout << "Usage: ./elf_packer input.so output.so" << std::endl;
        return 1;
    }
    encrypt_section(argv[1], argv[2]);
    return 0;
}
