import sys
import os
import subprocess
from backend.celery_worker import process_binary

def test_full_flow():
    test_file = "tests/test_app_c.c"
    task_id = "test_task_123"

    print(f"[*] Starting integration test for {test_file}")

    # Ensure directories exist
    os.makedirs("backend/processed", exist_ok=True)
    os.makedirs("backend/uploads", exist_ok=True)

    # Simulate the Celery task call
    result = process_binary(task_id, test_file, "test_app_c.c", "{}")

    if result["status"] == "success":
        output_binary = f"backend/processed/protected_{task_id}"
        print(f"[+] Protection successful. Output: {output_binary}")

        # Verify execution
        print("[*] Verifying execution of protected binary...")
        try:
            # Note: We need to use absolute path or correctly relative one for execution
            proc = subprocess.run([f"./{output_binary}"], capture_output=True, text=True, check=True)
            output = proc.stdout
            print(f"--- Output ---\n{output}--------------")

            if "[VM] Decrypting and printing sensitive data:" in output:
                print("[SUCCESS] VMP logic executed correctly!")
            else:
                print("[FAILURE] VMP logic not found in output.")

            # Verify string hiding
            print("[*] Verifying string hiding...")
            strings_proc = subprocess.run(["strings", output_binary], capture_output=True, text=True)
            if "super secret" not in strings_proc.stdout:
                print("[SUCCESS] Sensitive strings are hidden!")
            else:
                print("[FAILURE] Sensitive strings are still visible!")

        except Exception as e:
            print(f"[ERROR] Execution failed: {e}")
    else:
        print(f"[FAILURE] Protection failed: {result}")

if __name__ == "__main__":
    test_full_flow()
