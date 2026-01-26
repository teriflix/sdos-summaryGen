import requests
import os
import json
from dotenv import load_dotenv

load_dotenv()

OLLAMA_BASE_URL = os.getenv("OLLAMA_BASE_URL", "http://127.0.0.1:11434")
CHAT_MODEL = os.getenv("CHAT_MODEL", "llama3.1:8b-instruct-q4_0")
EMBED_MODEL = os.getenv("EMBED_MODEL", "nomic-embed-text:latest")

def check_ollama_status():
    """Checks if Ollama is running."""
    try:
        response = requests.get(f"{OLLAMA_BASE_URL}/")
        return response.status_code == 200
    except requests.exceptions.ConnectionError:
        return False

def generate_embedding(text):
    """Generates embeddings for a given text."""
    url = f"{OLLAMA_BASE_URL}/api/embeddings"
    payload = {
        "model": EMBED_MODEL,
        "prompt": text
    }
    try:
        print(f"📡 Generating embedding... (len: {len(text)})")
        response = requests.post(url, json=payload)
        response.raise_for_status()
        print(f"✅ Embedding generated.")
        return response.json()["embedding"]
    except Exception as e:
        print(f"❌ Error generating embedding: {e}")
        return None

def chat_completion(system_prompt, messages, stream=False):
    """Sends a chat request to Ollama."""
    url = f"{OLLAMA_BASE_URL}/api/chat"
    
    # Ensure messages have roles
    formatted_messages = []
    if system_prompt:
        formatted_messages.append({"role": "system", "content": system_prompt})
        
    for msg in messages:
        if "role" not in msg:
            msg["role"] = "user"
        formatted_messages.append(msg)

    payload = {
        "model": CHAT_MODEL,
        "messages": formatted_messages,
        "stream": stream
    }
    try:
        print(f"📡 Sending request to: {url}")
        print(f"📤 Full Payload:\n{json.dumps(payload, indent=2)}")
        
        response = requests.post(url, json=payload)
        response.raise_for_status()
        
        print(f"✅ Success: {url} | Status: {response.status_code}")
        
        if stream:
            # Handle streaming if needed later
            return response
        else:
            content = response.json()["message"]["content"]
            print(f"📥 Received Content:\n{content}\n{'-'*40}")
            return content
    except Exception as e:
        print(f"❌ Error in chat completion: {e}")
        return None
