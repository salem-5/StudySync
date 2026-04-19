import socket
import json
import threading
import time
import logging
import traceback
import re
import os
from dotenv import load_dotenv

import google.generativeai as genai
from google.api_core.exceptions import ResourceExhausted

# Load environment variables from the .env file
load_dotenv()

logging.basicConfig(
    level=logging.DEBUG,
    format="%(asctime)s [%(levelname)s] [%(threadName)s] %(message)s",
)

logger = logging.getLogger("StudySync-AI")

# Fetch API Key from environment variables
API_KEY = os.getenv("GEMINI_API_KEY")
if not API_KEY:
    logger.error("GEMINI_API_KEY is missing from the environment variables. Please check your .env file.")
    exit(1)

genai.configure(api_key=API_KEY)
model = genai.GenerativeModel("gemini-2.5-flash")

# Fetch HOST and PORT from environment variables, falling back to defaults if not found
HOST = os.getenv("HOST", '127.0.0.1')
PORT = int(os.getenv("PORT", 2570))

MASTER_PROMPT = """
You are the StudySync AI Tutor, a highly intelligent, encouraging, and helpful educational assistant built directly into the StudySync app.
StudySync is a collaborative study planner and task management application for students.

Your role:
1. Act as a supportive tutor. Explain concepts clearly and concisely.
2. If the user attaches tasks to their message, use that context to give highly specific advice regarding their workload, tags, or completion status.
3. Keep your answers clean and easy to read using plain text only. Do not use any markdown formatting at all except for code blocks.
4. ALWAYS wrap any code examples you provide inside standard markdown code blocks (for example: ```cpp ... ```), including the language alias for syntax highlighting.
5. Do not hallucinate features about the app.
"""

def handle_client(conn, addr):
    logger.info(f"Connection established from {addr}")
    buffer = ""

    try:
        while True:
            data = conn.recv(4096)

            if not data:
                logger.info(f"Client {addr} disconnected")
                break

            decoded = data.decode('utf-8', errors='replace')
            logger.debug(f"[RAW DATA] {decoded}")

            buffer += decoded

            while '\n' in buffer:
                line, buffer = buffer.split('\n', 1)

                if line.strip():
                    logger.debug(f"[LINE RECEIVED] {line}")
                    process_ai_request(conn, line.strip(), addr)

    except Exception as e:
        logger.error(f"Connection error with {addr}: {e}")
        logger.debug(traceback.format_exc())

    finally:
        conn.close()
        logger.info(f"Connection closed for {addr}")


def process_ai_request(conn, json_str, addr):
    start_time = time.time()

    try:
        logger.debug(f"[JSON INPUT] {json_str}")
        req = json.loads(json_str)

        user_id = req.get("user_id")
        req_id = req.get("req_id", -1)
        prompt = req.get("current_message", "")
        message_history = req.get("message_history", [])
        attachments = req.get("attached_tasks", [])

        logger.info(f"[REQUEST] user_id={user_id}, req_id={req_id}")
        logger.debug(f"[PROMPT] {prompt}")
        logger.debug(f"[ATTACHMENTS] {attachments}")

        context_str = ""
        if message_history:
            context_str += "\n[Conversation History]:\n"
            for msg in message_history:
                role = "User" if msg.get("role") == "user" else "AI Tutor"
                context_str += f"{role}: {msg.get('content')}\n"

        if attachments:
            context_str += "\n[Attached Tasks Data]:\n"
            for t in attachments:
                context_str += (
                    f"- Task ID {t.get('id')}: '{t.get('title')}' "
                    f"(Tag: {t.get('tag')}, Group ID: {t.get('groupId')})\n"
                )

        full_prompt = f"{MASTER_PROMPT}\n{context_str}\nUser asks: {prompt}"

        logger.debug(f"[FULL PROMPT LENGTH] {len(full_prompt)}")

        ai_start = time.time()
        response = model.generate_content(full_prompt)
        ai_time = time.time() - ai_start

        text_response = getattr(response, "text", None)

        if not text_response:
            raise ValueError("Gemini returned empty response")

        logger.info(f"[AI RESPONSE RECEIVED] in {ai_time:.2f}s")
        logger.debug(f"[AI OUTPUT] {text_response}")

        res_payload = {
            "user_id": user_id,
            "req_id": req_id,
            "status": "success",
            "message": text_response
        }

    except ResourceExhausted as e:
        logger.warning(f"[RATE LIMIT] {e}")

        match = re.search(r"retry in ([0-9.]+)s", str(e))
        wait_time = match.group(1) if match else "a few seconds"

        try:
            parsed = json.loads(json_str)
        except:
            parsed = {}

        res_payload = {
            "user_id": parsed.get("user_id", -1),
            "req_id": parsed.get("req_id", -1),
            "status": "success",
            "message": f"Rate limit reached. Try again in {wait_time} seconds."
        }

    except Exception as e:
        logger.error(f"[AI ERROR] {e}")
        logger.debug(traceback.format_exc())

        try:
            parsed = json.loads(json_str)
        except:
            parsed = {}

        res_payload = {
            "user_id": parsed.get("user_id", -1),
            "req_id": parsed.get("req_id", -1),
            "status": "error",
            "message": f"AI Generation Failed: {str(e)}"
        }

    try:
        payload_str = json.dumps(res_payload)
        logger.debug(f"[SENDING] {payload_str}")

        conn.sendall((payload_str + '\n').encode('utf-8'))

        total_time = time.time() - start_time
        logger.info(f"[REQUEST COMPLETE] req_id={res_payload['req_id']} in {total_time:.2f}s")

    except Exception as e:
        logger.error(f"[SEND ERROR] {e}")
        logger.debug(traceback.format_exc())


def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        s.bind((HOST, PORT))
        s.listen()

        logger.info(f"AI Backend listening on {HOST}:{PORT}")
        logger.info("Waiting for C++ server...")

        while True:
            conn, addr = s.accept()
            thread = threading.Thread(
                target=handle_client,
                args=(conn, addr),
                daemon=True
            )
            thread.start()


if __name__ == "__main__":
    start_server()