import urllib.request
import urllib.parse
import json
import time
import sys
import os

BASE_URL = "http://localhost:8080"
FILEPATH = os.path.abspath("test.fountain")

def make_request(endpoint, data=None, method='POST'):
    url = f"{BASE_URL}{endpoint}"
    headers = {'Content-Type': 'application/json'}
    
    if data:
        json_data = json.dumps(data).encode('utf-8')
        req = urllib.request.Request(url, data=json_data, headers=headers, method=method)
    else:
        req = urllib.request.Request(url, headers=headers, method=method)
        
    try:
        with urllib.request.urlopen(req) as response:
            return json.loads(response.read().decode('utf-8'))
    except urllib.error.HTTPError as e:
        print(f"HTTP Error {e.code}: {e.read().decode('utf-8')}")
        return None
    except Exception as e:
        print(f"Error: {e}")
        return None

def test_start_session():
    print("\n[TEST] Start Session...")
    payload = {
        "filepath": FILEPATH,
        "context_window": 8192,
        "model_name": "llama3.1:8b-instruct-q4_0"
    }
    res = make_request("/session/start", payload)
    if res:
        print(f"Success: {res}")
        return res.get("session_id")
    return None

def test_scene_summary(session_id):
    print("\n[TEST] Scene Summary (Scene 1)...")
    payload = {
        "session_id": session_id,
        "scene_id": 1
    }
    res = make_request("/summary/scene", payload)
    if res:
        print(f"Success: {list(res.keys())}")
    
    print("\n[TEST] Scene Summary with System Prompt Override...")
    payload_override = {
        "session_id": session_id,
        "scene_id": 2,
        "system_prompt": "You are a pirate. Summarize the scene in pirate speak."
    }
    res_override = make_request("/summary/scene", payload_override)
    if res_override:
        print(f"Success Override: {list(res_override.keys())}")
        if 'summary' in res_override:
             print(f"Override Summary Snippet: {res_override['summary'][:50]}...")

def test_get_session(session_id):
    print(f"\n[TEST] Get Session Metadata ({session_id})...")
    res = make_request(f"/session/{session_id}", method='GET')
    if res:
        print(f"Success. Story Summary exists? {bool(res.get('storySummary'))}")
        print(f"Scene count: {len(res.get('scenes', []))}")
        print(f"Character count: {len(res.get('characters', {}))}")

if __name__ == "__main__":
    time.sleep(2) 
    
    session_id = test_start_session()
    if session_id:
        test_scene_summary(session_id)
        test_get_session(session_id)
    else:
        print("Skipping further tests due to session failure.")
