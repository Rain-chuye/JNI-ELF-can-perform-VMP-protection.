from fastapi import FastAPI, UploadFile, File, Form
from fastapi.middleware.cors import CORSMiddleware
from fastapi.responses import FileResponse
import uuid
import os
from celery_worker import process_binary

app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_methods=["*"],
    allow_headers=["*"],
)

UPLOAD_DIR = "uploads"
PROCESSED_DIR = "processed"
os.makedirs(UPLOAD_DIR, exist_ok=True)
os.makedirs(PROCESSED_DIR, exist_ok=True)

@app.post("/upload")
async def upload_file(file: UploadFile = File(...), options: str = Form("{}")):
    task_id = str(uuid.uuid4())
    file_ext = os.path.splitext(file.filename)[1]
    file_path = os.path.join(UPLOAD_DIR, f"{task_id}{file_ext}")
    with open(file_path, "wb") as f:
        f.write(await file.read())

    # Trigger Celery task
    process_binary.delay(task_id, file_path, file.filename, options)
    return {"task_id": task_id}

@app.get("/status/{task_id}")
async def get_status(task_id: str):
    processed_path = os.path.join(PROCESSED_DIR, f"protected_{task_id}")
    if os.path.exists(processed_path):
        return {"status": "completed", "download_url": f"/download/{task_id}"}
    return {"status": "processing"}

@app.get("/download/{task_id}")
async def download_file(task_id: str):
    processed_path = os.path.join(PROCESSED_DIR, f"protected_{task_id}")
    if os.path.exists(processed_path):
        return FileResponse(processed_path, filename=f"protected_binary")
    return {"error": "File not found"}

if __name__ == "__main__":
    import uvicorn
    uvicorn.run(app, host="0.0.0.0", port=8000)
