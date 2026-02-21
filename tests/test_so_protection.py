import os
import shutil
from backend.celery_worker import process_binary

def test_so_flow():
    # Create a dummy .so file
    dummy_so = "tests/dummy.so"
    with open(dummy_so, "wb") as f:
        f.write(b"ELF_HEADER_DATA_AND_SECRET_STRINGS")

    task_id = "test_so_task"
    print(f"[*] Testing .so protection flow...")

    result = process_binary(task_id, dummy_so, "dummy.so", "{}")

    if result["status"] == "success":
        output_path = f"backend/processed/protected_{task_id}"
        if os.path.exists(output_path):
            print(f"[SUCCESS] .so file protected and saved to {output_path}")
            # Verify XOR (key 0xAA)
            with open(output_path, "rb") as f:
                data = f.read()
                if data[0] == ord('E') ^ 0xAA:
                    print("[SUCCESS] XOR Encryption verified on .so file!")
                else:
                    print("[FAILURE] XOR Encryption failed!")
        else:
            print("[FAILURE] Output file not found!")
    else:
        print(f"[FAILURE] Task failed: {result}")

if __name__ == "__main__":
    os.makedirs("backend/processed", exist_ok=True)
    test_so_flow()
