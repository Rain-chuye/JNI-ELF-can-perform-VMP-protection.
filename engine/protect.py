import sys
import re
import random
import os

def xor_encrypt(data, key):
    return bytes([b ^ key for b in data])

def ir_escape(data):
    return "".join(f"\\{b:02x}" for b in data)

class IRProtector:
    def __init__(self, content, options=None):
        self.content = content
        self.options = options or {}
        self.strings = []

    def rename_registers(self, body):
        body = re.sub(r'%(\d+)', r'%r\1', body)
        body = re.sub(r'^(\d+):', r'r\1:', body, flags=re.MULTILINE)
        return body

    def protect_strings(self):
        if not self.options.get("strings", True): return
        pattern = r'(@\.str[0-9.]*|@__const\.[^ ]+) = private unnamed_addr constant \[(\d+) x i8\] c"(.*?)", align \d+'
        def replacer(match):
            var_name, length, raw_content = match.groups()
            processed_bytes = bytearray()
            i = 0
            while i < len(raw_content):
                if raw_content[i] == '\\' and i + 2 < len(raw_content):
                    try:
                        processed_bytes.append(int(raw_content[i+1:i+3], 16))
                        i += 3
                    except ValueError:
                        processed_bytes.append(ord(raw_content[i]))
                        i += 1
                else:
                    processed_bytes.append(ord(raw_content[i]))
                    i += 1
            key = random.randint(1, 255)
            encrypted = xor_encrypt(processed_bytes, key)
            self.strings.append((var_name, len(processed_bytes), key))
            return f'{var_name} = private unnamed_addr global [{len(processed_bytes)} x i8] c"{ir_escape(encrypted)}", align 1'
        self.content = re.sub(pattern, replacer, self.content)

    def instruction_substitution(self, body):
        if not self.options.get("sub", True): return body
        lines = body.split('\n')
        new_lines = []
        for line in lines:
            # Substitution for add: a + b -> (a ^ b) + 2*(a & b)
            # This is hard to do purely with regex in LLVM IR without breaking types.
            # Let's do a simpler one: a + b -> a - (-b)
            # Actually, let's do: a ^ b -> (a & ~b) | (~a & b)
            # For this PoC, we will focus on 'add' and 'xor'
            m = re.search(r'(%[a-zA-Z0-9._]+) = add (i\d+) (.*?), (.*)', line)
            if m:
                res, ty, op1, op2 = m.groups()
                tmp1 = f"{res}_sub1"
                tmp2 = f"{res}_sub2"
                new_lines.append(f"  {tmp1} = sub {ty} 0, {op2}")
                new_lines.append(f"  {res} = sub {ty} {op1}, {tmp1}")
                continue
            new_lines.append(line)
        return "\n".join(new_lines)

    def flatten_cfg(self):
        if not self.options.get("fla", True): return
        output = []
        pos = 0
        pattern = re.compile(r'^define (.*?) @(.*?)\((.*?)\) (.*?) \{(.*?)\n\}', re.DOTALL | re.MULTILINE)
        for match in pattern.finditer(self.content):
            output.append(self.content[pos:match.start()])
            prefix, func_name, args, attrs, body = match.groups()
            args = self.rename_registers(args)
            body = self.rename_registers(body)
            body = self.instruction_substitution(body)

            if "llvm." in func_name:
                output.append(match.group(0))
                pos = match.end(); continue

            blocks = []
            current_block_label = "entry"
            current_block_lines = []
            for line in body.split('\n'):
                lstrip = line.strip()
                label_match = re.match(r'^([a-zA-Z0-9._]+):', lstrip)
                if label_match:
                    if current_block_lines: blocks.append((current_block_label, current_block_lines))
                    current_block_label, current_block_lines = label_match.group(1), []
                elif lstrip: current_block_lines.append(line)
            if current_block_lines: blocks.append((current_block_label, current_block_lines))

            if len(blocks) < 2:
                output.append(f'define {prefix} @{func_name}({args}) {attrs} {{\n' + body + "\n}")
                pos = match.end(); continue

            block_ids = {name: random.randint(10000, 999999) for name, _ in blocks}
            new_body, allocas, rest = [], [], []
            for line in blocks[0][1]:
                if "alloca" in line: allocas.append(line)
                else: rest.append(line)

            new_body.extend(allocas)
            new_body.append(f"  %v_state = alloca i32, align 4")
            new_body.append(f"  store i32 {block_ids[blocks[0][0]]}, ptr %v_state, align 4")
            new_body.append(f"  br label %v_dispatcher")
            new_body.append(f"\nv_dispatcher:")
            new_body.append(f"  %curr_v_state = load i32, ptr %v_state, align 4")
            switch_cmd = f"  switch i32 %curr_v_state, label %v_default_dest ["
            for name, block_id in block_ids.items(): switch_cmd += f"\n    i32 {block_id}, label %v_block_{name}"
            new_body.append(switch_cmd + "\n  ]")
            new_body.append(f"\nv_default_dest:\n  ret {'void' if 'void' in prefix else 'i32 0'}")
            blocks[0] = (blocks[0][0], rest)
            for name, lines in blocks:
                new_body.append(f"\nv_block_{name}:")
                if not lines: new_body.append(f"  br label %v_dispatcher"); continue
                last_line, other_lines = lines[-1].strip(), lines[:-1]
                new_body.extend(other_lines)
                if last_line.startswith("br i1"):
                    m = re.search(r'br i1 (.*?), label %(.*?), label %(.*)', last_line)
                    if m:
                        cond, t_l, f_l = m.groups()
                        f_l = f_l.split(',')[0].strip()
                        new_body.append(f"  %next_v_state_{name} = select i1 {cond}, i32 {block_ids.get(t_l, 0)}, i32 {block_ids.get(f_l, 0)}")
                        new_body.append(f"  store i32 %next_v_state_{name}, ptr %v_state, align 4")
                        new_body.append(f"  br label %v_dispatcher")
                    else: new_body.append(last_line)
                elif last_line.startswith("br label"):
                    m = re.search(r'br label %(.*)', last_line)
                    if m:
                        dest = m.group(1).strip()
                        new_body.append(f"  store i32 {block_ids.get(dest, 0)}, ptr %v_state, align 4")
                        new_body.append(f"  br label %v_dispatcher")
                    else: new_body.append(last_line)
                else:
                    new_body.append(last_line)
                    if "ret" not in last_line and "br " not in last_line:
                        new_body.append(f"  br label %v_dispatcher")
            output.append(f'define {prefix} @{func_name}({args}) {attrs} {{\n' + "\n".join(new_body) + "\n}")
            pos = match.end()
        output.append(self.content[pos:]); self.content = "".join(output)

    def process(self, output_file):
        self.protect_strings()
        self.flatten_cfg()
        decryption_logic = """
declare void @decrypt_data(ptr, i64, i8)
define void @__vmp_init_strings() {
entry:
"""
        for var_name, length, key in self.strings:
            decryption_logic += f"  call void @decrypt_data(ptr {var_name}, i64 {length}, i8 {key})\n"
        decryption_logic += "  ret void\n}\n@llvm.global_ctors = appending global [1 x { i32, ptr, ptr }] [{ i32, ptr, ptr } { i32 65535, ptr @__vmp_init_strings, ptr null }]\n"
        self.content += decryption_logic
        with open(output_file, 'w') as f: f.write(self.content)

if __name__ == "__main__":
    if len(sys.argv) < 3:
        print("Usage: protect.py input.ll output.ll [options_json]")
        sys.exit(1)

    input_file = sys.argv[1]
    output_file = sys.argv[2]
    options = {}
    if len(sys.argv) > 3:
        import json
        try:
            options = json.loads(sys.argv[3])
        except:
            pass

    with open(input_file, 'r') as f: content = f.read()
    IRProtector(content, options).process(output_file)
