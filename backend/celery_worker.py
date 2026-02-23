from celery import Celery
import os
import subprocess
import json

celery_app = Celery('tasks', broker='redis://localhost:6379/0')

@celery_app.task
def process_binary(task_id, file_path, original_filename, options_json):
    options = json.loads(options_json)
    base_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    processed_dir = os.path.join(base_dir, "backend", "processed")
    os.makedirs(processed_dir, exist_ok=True)

    engine_path = os.path.join(base_dir, "engine", "protect.py")
    binary_engine_path = os.path.join(base_dir, "engine", "binary_protect.py")
    runtime_src = os.path.join(base_dir, "runtime", "runtime.c")

    output_binary = os.path.join(processed_dir, f"protected_{task_id}")
    metadata_path = os.path.join(processed_dir, f"metadata_{task_id}.json")

    arch = options.get("arch", "x86_64")
    clang_target = ""
    if arch == "aarch64": clang_target = "--target=aarch64-linux-gnu"
    elif arch == "arm": clang_target = "--target=arm-linux-gnueabi"

    try:
        if original_filename.endswith('.so'):
            # Binary-level protection (ELF/JNI Hardening)
            subprocess.run(["python3", binary_engine_path, file_path, output_binary, options_json], check=True)
            security_score = 85
        else:
            # Source-level protection (LLVM IR)
            ll_path = f"{file_path}.ll"
            protected_ll_path = f"{file_path}_protected.ll"
            runtime_obj = f"{file_path}_runtime.o"

            subprocess.run(["clang", clang_target, "-S", "-emit-llvm", "-O0", file_path, "-o", ll_path], check=True)
            # Pass options to IR engine
            subprocess.run(["python3", engine_path, ll_path, protected_ll_path, options_json], check=True)
            subprocess.run(["clang", clang_target, "-c", runtime_src, "-o", runtime_obj], check=True)
            subprocess.run(["clang", clang_target, protected_ll_path, runtime_obj, "-o", output_binary, "-lm"], check=True)
            security_score = 95

        metadata = {
            "task_id": task_id,
            "original_filename": original_filename,
            "security_score": security_score,
            "protections_applied": [k for k,v in options.items() if v and k != "arch"],
            "arch": arch
        }
        with open(metadata_path, "w") as f: json.dump(metadata, f)
        return {"status": "success", "task_id": task_id}
    except Exception as e:
        print(f"Error: {e}")
        return {"status": "error", "message": str(e)}
