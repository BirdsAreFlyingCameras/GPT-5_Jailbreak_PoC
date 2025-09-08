from fastapi import FastAPI, Form
from fastapi.responses import PlainTextResponse
from fastapi.middleware.cors import CORSMiddleware
import uvicorn

app = FastAPI()

# Allow your frontend origin(s). During testing, "*" means any.
app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],   # or replace "*" with ["http://127.0.0.1:5500"]
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

current_task = ""

@app.get("/task", response_class=PlainTextResponse)
def get_task():
    return current_task

@app.post("/task")
def set_task(
    ip: str = Form(...),
    port: int = Form(...),
    message: str = Form(...),
    count: int = Form(...),
    interval: float = Form(...)
):
    global current_task
    current_task = f"{ip}:{port}:{message}:{count}:{interval}"
    return {"status": "ok", "task_set": current_task}

@app.post("/clear")
def clear_task():
    global current_task
    current_task = ""
    return {"status": "ok", "task_cleared": True}

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8080)
