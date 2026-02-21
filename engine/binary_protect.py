import sys
import os

def protect_so(input_path, output_path):
    """
    Simple Binary-level protection for .so files.
    This PoC encrypts the entire file content and wraps it with a simple XOR,
    simulating a 'packer' or 'static string hider' at the binary level.
    """
    print(f"[*] Protecting binary: {input_path}")
    with open(input_path, 'rb') as f:
        data = f.read()

    # In a real binary protector, we would:
    # 1. Parse ELF header
    # 2. Find .rodata or .data segments
    # 3. Encrypt those segments
    # 4. Inject a decryption stub into a new segment or the entry point.

    # For this MVP/PoC, we will simulate the encryption of sensitive sections
    # by XORing a portion of the file that typically contains strings.

    protected_data = bytearray(data)
    key = 0xAA

    # XORing data (simulating section encryption)
    for i in range(len(protected_data)):
        protected_data[i] ^= key

    with open(output_path, 'wb') as f:
        f.write(protected_data)

    print(f"[+] Protected binary saved to: {output_path}")

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python binary_protect.py input.so output.so")
    else:
        protect_so(sys.argv[1], sys.argv[2])
