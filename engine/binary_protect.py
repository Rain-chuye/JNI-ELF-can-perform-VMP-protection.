import sys
import lief
import random
import os
import string

def random_name(length=12):
    return ''.join(random.choice(string.ascii_letters) for _ in range(length))

def protect_so(input_path, output_path, options=None):
    options = options or {}
    binary = lief.parse(input_path)

    # Simple and safe symbol obfuscation:
    # Only rename symbols that are NOT in the dynamic symbol table
    dyn_sym_names = {s.name for s in binary.dynamic_symbols}

    if options.get("sym_obf", True):
        for symbol in binary.symbols:
            if symbol.name and symbol.name not in dyn_sym_names and not symbol.name.startswith("_"):
                symbol.name = random_name()

    if options.get("sec_enc", True):
        section = binary.get_section(".rodata")
        if section:
            key = 0xAA
            section.content = [b ^ key for b in section.content]

    binary.write(output_path)
    os.chmod(output_path, 0o755)
    print(f"[+] Protected {input_path}")

if __name__ == "__main__":
    import json
    options = json.loads(sys.argv[3]) if len(sys.argv) > 3 else {}
    protect_so(sys.argv[1], sys.argv[2], options)
