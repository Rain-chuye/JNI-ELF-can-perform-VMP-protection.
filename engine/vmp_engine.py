import sys
import re
import os
import struct

# VM Opcodes (must match vmp_runtime.h)
OP_HALT = 0x00
OP_PUSH_CONST = 0x01
OP_PUSH_VAR = 0x02
OP_POP_VAR = 0x03
OP_ADD = 0x04
OP_SUB = 0x05
OP_MUL = 0x06
OP_DIV = 0x07
OP_AND = 0x08
OP_OR  = 0x09
OP_XOR = 0x0A
OP_NOT = 0x0B
OP_JMP = 0x0C
OP_JZ  = 0x0D
OP_JNZ = 0x0E
OP_CALL_NATIVE = 0x0F
OP_RET = 0x10
OP_LOAD = 0x11
OP_PRINT = 0x13
OP_STORE = 0x12

def xor_encrypt(data, key=0xAA):
    return bytes([b ^ key for b in data])

def ir_escape(data):
    return "".join(f"\\{b:02x}" for b in data)

class VMProjector:
    def __init__(self, content):
        self.content = content
        self.bytecode_defs = []
        self.strings_to_decrypt = []

    def encrypt_strings(self):
        # Pattern for LLVM IR string constants
        pattern = r'(@\.str\d*) = private unnamed_addr constant \[(\d+) x i8\] c"(.*?)", align 1'

        def replacer(match):
            var_name = match.group(1)
            length = int(match.group(2))
            raw_content = match.group(3)

            # Process escapes
            processed_bytes = bytearray()
            i = 0
            while i < len(raw_content):
                if raw_content[i] == '\\' and i + 2 < len(raw_content):
                    try:
                        processed_bytes.append(int(raw_content[i+1:i+3], 16))
                        i += 3
                    except:
                        processed_bytes.append(ord(raw_content[i]))
                        i += 1
                else:
                    processed_bytes.append(ord(raw_content[i]))
                    i += 1

            key = 0x66
            encrypted = xor_encrypt(processed_bytes, key)
            self.strings_to_decrypt.append((var_name, len(processed_bytes), key))

            # Change to 'global' so we can decrypt in place
            return f'{var_name} = private unnamed_addr global [{len(processed_bytes)} x i8] c"{ir_escape(encrypted)}", align 1'

        self.content = re.sub(pattern, replacer, self.content)

    def virtualize_functions(self):
        # Find all functions marked for virtualization or all functions if auto-vmp is enabled
        # For this tool, we'll virtualize functions that have a body
        func_pattern = r'define dso_local (.*?) @(.*?)\((.*?)\) #(.*?) \{(.*?)\n\}'

        def func_replacer(match):
            ret_type = match.group(1)
            func_name = match.group(2)
            args_str = match.group(3)
            attr = match.group(4)
            body = match.group(5)

            if func_name.startswith("__vmp"): return match.group(0) # Don't vmp our own logic
            if "vmp_interpreter" in func_name: return match.group(0)

            print(f"[*] Virtualizing function: {func_name}")

            # Simple bytecode generation for the body
            # In a real tool, this would parse the IR instructions.
            # Here we simulate it by generating a PUSH_CONST + RET for logic
            # Or if it's a known math function, we map it.

            bytecode = bytearray()
            # Simple simulation: return 42 ^ input if it's a simple function
            # For this MVP, we just generate a HALT to show it's virtualized
            bytecode.append(OP_PUSH_CONST)
            bytecode.extend(struct.pack("<q", 42))
            bytecode.append(OP_PRINT) # Debug print
            bytecode.append(OP_RET)

            bc_var_name = f"@bc_{func_name}"
            encrypted_bc = xor_encrypt(bytecode, 0x77)
            self.bytecode_defs.append(f'{bc_var_name} = private constant [{len(encrypted_bc)} x i8] c"{ir_escape(encrypted_bc)}", align 1')

            # Create the VMP stub
            # We need to collect arguments into an array
            # For simplicity, we assume up to 4 arguments
            vmp_stub = f"""
  %args = alloca [4 x ptr]
  %bytecode_ptr = getelementptr inbounds [{len(encrypted_bc)} x i8], ptr {bc_var_name}, i64 0, i64 0
  %args_ptr = getelementptr inbounds [4 x ptr], ptr %args, i64 0, i64 0
  call void @vmp_interpreter(ptr %bytecode_ptr, ptr %args_ptr)
"""
            if ret_type == "void":
                vmp_stub += "  ret void"
            else:
                # Dummy return for non-void
                vmp_stub += f"  ret {ret_type} 0"

            return f'define dso_local {ret_type} @{func_name}({args_str}) #{attr} {{{vmp_stub}\n}}'

        self.content = re.sub(func_pattern, func_replacer, self.content, flags=re.DOTALL)

    def inject_runtime_init(self):
        init_logic = """
declare void @vmp_decrypt(ptr, i64, i8)
declare void @vmp_interpreter(ptr, ptr)

define void @__vmp_init() {
entry:
"""
        for var_name, length, key in self.strings_to_decrypt:
            init_logic += f"  call void @vmp_decrypt(ptr {var_name}, i64 {length}, i8 {key})\n"

        init_logic += "  ret void\n}\n"
        init_logic += """
@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init, ptr null }]
"""
        self.content += "\n" + "\n".join(self.bytecode_defs) + "\n" + init_logic

    def process(self):
        self.encrypt_strings()
        self.virtualize_functions()
        self.inject_runtime_init()
        return self.content

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: python vmp_engine.py input.ll output.ll")
        sys.exit(1)

    with open(sys.argv[1], 'r') as f:
        ir = f.read()

    projector = VMProjector(ir)
    protected_ir = projector.process()

    with open(sys.argv[2], 'w') as f:
        f.write(protected_ir)
