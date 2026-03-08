import subprocess
import os
import requests
import time

BACKEND_URL = "http://localhost:8000"

def test_source_to_elf():
    print("[*] Testing Source to Protected ELF...")
    test_file = "tests/complex_test.c"
    with open(test_file, 'rb') as f:
        res = requests.post(f"{BACKEND_URL}/upload",
                          files={'file': f},
                          data={'options': '{"arch":"x86_64"}'})
    task_id = res.json()['task_id']
    while True:
        res = requests.get(f"{BACKEND_URL}/status/{task_id}")
        if res.json()['status'] == 'completed': break
        time.sleep(1)
    res = requests.get(f"{BACKEND_URL}/download/{task_id}")
    protected_path = f"tests/protected_{task_id}"
    with open(protected_path, 'wb') as f: f.write(res.content)
    os.chmod(protected_path, 0o755)
    print("[*] Running protected binary...")
    output = subprocess.check_output([f"./{protected_path}"], stderr=subprocess.STDOUT).decode()
    print("[+] Output received:")
    print(output)
    if "Starting complex test" in output:
        print("[SUCCESS] Protected ELF runs and produces output!")
    else:
        print("[FAILURE] Output mismatch!")

if __name__ == "__main__":
    test_source_to_elf()
