from celery import Celery
import os
import subprocess
import shutil

celery_app = Celery('tasks', broker='redis://localhost:6379/0')

@celery_app.task
def process_binary(task_id, file_path, original_filename, options):
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    processed_dir = os.path.join(base_dir, "backend", "processed")
    os.makedirs(processed_dir, exist_ok=True)

    engine_path = os.path.join(base_dir, "engine", "protect.py")
    binary_engine_path = os.path.join(base_dir, "engine", "binary_protect.py")
    runtime_src = os.path.join(base_dir, "runtime", "runtime.c")
    runtime_obj = os.path.join(base_dir, "runtime", "runtime.o")

    output_binary = os.path.join(processed_dir, f"protected_{task_id}")

    try:
        if original_filename.endswith('.so'):
            # Binary-level protection for .so files
            subprocess.run(["python3", binary_engine_path, file_path, output_binary], check=True)
        else:
            # Source-level protection (LLVM IR) for C/C++ files
            ll_path = f"{file_path}.ll"
            protected_ll_path = f"{file_path}_protected.ll"

            # 1. Compile to LLVM IR
            subprocess.run(["clang", "-S", "-emit-llvm", file_path, "-o", ll_path], check=True)

            # 2. Run Protection Engine
            subprocess.run(["python3", engine_path, ll_path, protected_ll_path], check=True)

            # 3. Compile Runtime and Link
            if not os.path.exists(runtime_obj):
                subprocess.run(["clang", "-c", runtime_src, "-o", runtime_obj], check=True)

            subprocess.run(["clang", protected_ll_path, runtime_obj, "-o", output_binary], check=True)

        return {"status": "success", "task_id": task_id}
    except Exception as e:
        print(f"Error during protection: {e}")
        return {"status": "error", "message": str(e)}
