import socket
import json
import threading
import time
import logging
import os
from dotenv import load_dotenv

from google import genai

load_dotenv()

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(message)s",
)
logger = logging.getLogger("StudySync-AI")

API_KEY = os.getenv("GEMINI_API_KEY")
if not API_KEY:
    exit(1)

client = genai.Client(api_key=API_KEY)
HOST = os.getenv("HOST", '127.0.0.1')
PORT = int(os.getenv("PORT", 2570))

MASTER_PROMPT = """
You are the StudySync AI Tutor, a highly intelligent, encouraging, and helpful educational assistant.
StudySync is a collaborative study planner and task management application for students.

Your role:
1. Act as a supportive tutor. Explain concepts clearly and concisely.
2. If the user attaches tasks, give specific advice regarding workload/tags.
3. Keep answers clean using plain text. Do not use markdown except for code blocks.
4. ALWAYS wrap code in standard markdown code blocks (e.g. ```cpp ... ```).
5. Do not hallucinate features about the app.
"""

MODEL_PRIORITY = [
    "gemini-3.1-pro-preview",
    "gemini-3.1-flash-lite-preview",
    "gemini-2.5-pro",
    "gemini-2.5-flash",
    "gemini-2.0-flash",
    "gemini-1.5-flash",
]

def try_models(full_prompt):
    """Returns a tuple of (response_text, model_name)."""
    for name in MODEL_PRIORITY:
        try:
            response = client.models.generate_content(model=name, contents=full_prompt)
            text = getattr(response, "text", None)
            if text:
                return text, name
        except Exception:
            continue
    raise RuntimeError("All models failed.")

def handle_client(conn, addr):
    conn.settimeout(120.0)
    buffer = ""
    try:
        while True:
            data = conn.recv(4096)
            if not data:
                break
            buffer += data.decode('utf-8', errors='replace')
            while '\n' in buffer:
                line, buffer = buffer.split('\n', 1)
                if line.strip():
                    process_ai_request(conn, line.strip())
    except Exception:
        pass
    finally:
        conn.close()

def process_ai_request(conn, json_str):
    try:
        req = json.loads(json_str)
        user_id = req.get("user_id")
        req_id = req.get("req_id", -1)
        prompt = req.get("current_message", "")
        message_history = req.get("message_history", [])
        attachments = req.get("attached_tasks", [])

        context_parts = [MASTER_PROMPT]
        if message_history:
            history_str = "\n[History]:\n" + "\n".join([f"{m.get('role')}: {m.get('content')}" for m in message_history])
            context_parts.append(history_str)
        if attachments:
            task_str = "\n[Tasks]:\n" + "\n".join([f"- {t.get('title')}" for t in attachments])
            context_parts.append(task_str)

        context_parts.append(f"\nUser: {prompt}")
        text_response, model_used = try_models("\n".join(context_parts))

        res_payload = {
            "user_id": user_id,
            "req_id": req_id,
            "status": "success",
            "message": text_response
        }
        logger.info(f"Request found and answered using model: {model_used}")

    except Exception:
        res_payload = {"status": "error", "message": "The AI is currently unavailable."}

    try:
        conn.sendall((json.dumps(res_payload) + '\n').encode('utf-8'))
    except Exception:
        pass

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5)
        logger.info(f"Server started on {HOST}:{PORT}")
        while True:
            conn, addr = s.accept()
            threading.Thread(target=handle_client, args=(conn, addr), daemon=True).start()

if __name__ == "__main__":
    start_server()