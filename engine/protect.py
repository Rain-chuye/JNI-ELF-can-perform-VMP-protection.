import sys
import re
import os

def xor_encrypt(data, key=0x42):
    return bytes([b ^ key for b in data])

def ir_escape(data):
    return "".join(f"\\{b:02x}" for b in data)

def process_ir(input_file, output_file):
    with open(input_file, 'r') as f:
        content = f.read()

    # --- 1. String Encryption ---
    strings_to_decrypt = []
    pattern = r'(@\.str\d*) = private unnamed_addr constant \[(\d+) x i8\] c"(.*?)", align 1'

    def string_replacer(match):
        var_name = match.group(1)
        length = int(match.group(2))
        raw_content = match.group(3)
        processed_bytes = bytearray()
        i = 0
        while i < len(raw_content):
            if raw_content[i] == '\\' and i + 2 < len(raw_content):
                processed_bytes.append(int(raw_content[i+1:i+3], 16))
                i += 3
            else:
                processed_bytes.append(ord(raw_content[i]))
                i += 1
        encrypted = xor_encrypt(processed_bytes)
        strings_to_decrypt.append((var_name, len(processed_bytes)))
        return f'{var_name} = private unnamed_addr global [{len(processed_bytes)} x i8] c"{ir_escape(encrypted)}", align 1'

    new_content = re.sub(pattern, string_replacer, content)

    # --- 2. Virtualization (VMP) ---
    bytecode = bytes([0x01, 0x02])
    encrypted_bytecode = xor_encrypt(bytecode, 0x77)
    vm_bytecode_def = f'@vm_code_secret_logic = private constant [2 x i8] c"{ir_escape(encrypted_bytecode)}", align 1'

    vmp_replacement = """
  %bytecode_ptr = getelementptr inbounds [2 x i8], ptr @vm_code_secret_logic, i64 0, i64 0
  call void @vm_interpreter(ptr %bytecode_ptr, ptr null)
  ret void
"""

    if "void @secret_logic()" in new_content:
        # Replace the entire body of the function
        new_content = re.sub(r'define dso_local void @secret_logic\(\) #\d+ \{(.*?)\}',
                             r'define dso_local void @secret_logic() #0 {' + vmp_replacement + '}',
                             new_content, flags=re.DOTALL)
        # Add the bytecode definition at the end (globals can be at the end)
        new_content += "\n" + vm_bytecode_def + "\n"

    # --- 3. Injections ---
    decryption_logic = """
declare void @decrypt_data(ptr, i64, i8)
declare void @vm_interpreter(ptr, ptr)

define void @__vmp_init_strings() {
entry:
"""
    for var_name, length in strings_to_decrypt:
        decryption_logic += f"  call void @decrypt_data(ptr {var_name}, i64 {length}, i8 66)\n"

    decryption_logic += "  ret void\n}\n"
    decryption_logic += """
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init_strings, ptr null }]
"""

    new_content += decryption_logic

    with open(output_file, 'w') as f:
        f.write(new_content)

if __name__ == "__main__":
    process_ir(sys.argv[1], sys.argv[2])
