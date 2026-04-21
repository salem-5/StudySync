import socket
import json
import threading
import logging
import os
from dotenv import load_dotenv

from google import genai
from google.genai import types

load_dotenv()

logging.basicConfig(
    level=logging.INFO,
    format="%(asctime)s - %(message)s",
)

logger = logging.getLogger("StudySync-AI")

logging.getLogger("google").setLevel(logging.WARNING)
logging.getLogger("google.genai").setLevel(logging.WARNING)
logging.getLogger("httpx").setLevel(logging.WARNING)
logging.getLogger("httpcore").setLevel(logging.WARNING)

API_KEY = os.getenv("GEMINI_API_KEY")
if not API_KEY:
    raise RuntimeError("Missing GEMINI_API_KEY")

client = genai.Client(api_key=API_KEY)

HOST = os.getenv("HOST", "127.0.0.1")
PORT = int(os.getenv("PORT", 2570))

MASTER_PROMPT = """
You are StudySync AI Tutor.
- Explain clearly and concisely
- Give advice on tasks if provided
- Use plain text, only code blocks for code
- Do not hallucinate app features
"""

FAST_MODEL = "gemini-3.1-flash-lite-preview"
FALLBACK_MODEL = "gemini-2.5-flash"

MAX_HISTORY = 6


def build_contents(prompt_text):
    return [
        types.Content(
            role="user",
            parts=[types.Part.from_text(text=prompt_text)]
        )
    ]


def generate_fast(full_prompt):
    for model_name in [FAST_MODEL, FALLBACK_MODEL]:
        try:
            contents = build_contents(full_prompt)

            stream = client.models.generate_content_stream(
                model=model_name,
                contents=contents
            )

            text = ""
            for chunk in stream:
                if chunk.text:
                    text += chunk.text

            if text:
                return text, model_name

        except Exception as e:
            logger.warning(f"{model_name} failed: {e}")

    raise RuntimeError("All models failed.")


def handle_client(conn, addr):
    conn.settimeout(120.0)
    buffer = ""
    try:
        while True:
            data = conn.recv(4096)
            if not data:
                break

            buffer += data.decode("utf-8", errors="replace")

            while "\n" in buffer:
                line, buffer = buffer.split("\n", 1)
                if line.strip():
                    process_ai_request(conn, line.strip())

    except Exception as e:
        logger.error(f"Client error: {e}", exc_info=True)
    finally:
        conn.close()

def process_ai_request(conn, json_str):
    try:
        try:
            req = json.loads(json_str)
        except json.JSONDecodeError as e:
            conn.sendall((json.dumps({
                "status": "error",
                "message": f"Invalid JSON: {str(e)}"
            }) + "\n").encode("utf-8"))
            return

        user_id = req.get("user_id")
        req_id = req.get("req_id", -1)
        prompt = req.get("current_message", "")
        message_history = req.get("message_history", [])
        attachments = req.get("attached_tasks", [])

        context_parts = [MASTER_PROMPT]

        if message_history:
            trimmed = message_history[-MAX_HISTORY:]
            history_str = "\n[History]:\n" + "\n".join(
                f"{m.get('role')}: {m.get('content')}" for m in trimmed
            )
            context_parts.append(history_str)
        if attachments:
            task_str = "\n[Tasks]:\n" + "\n".join(
                f"- {t.get('title')}" for t in attachments
            )
            context_parts.append(task_str)

        context_parts.append(f"\nUser: {prompt}")
        full_prompt = "\n".join(context_parts)

        text_response, model_used = generate_fast(full_prompt)

        res_payload = {
            "user_id": user_id,
            "req_id": req_id,
            "status": "success",
            "message": text_response
        }

        logger.info(f"Answered using: {model_used}")

    except Exception as e:
        logger.error(f"Processing error: {e}", exc_info=True)

        res_payload = {
            "status": "error",
            "message": f"Backend error: {type(e).__name__}: {str(e)}"
        }

    try:
        conn.sendall((json.dumps(res_payload) + "\n").encode("utf-8"))
    except Exception as e:
        logger.error(f"Send error: {e}")


def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((HOST, PORT))
        s.listen(5)
        logger.info(f"Server started on {HOST}:{PORT}")
        while True:
            conn, addr = s.accept()
            threading.Thread(
                target=handle_client,
                args=(conn, addr),
                daemon=True
            ).start()


if __name__ == "__main__":
    start_server()