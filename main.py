import os
import datetime
from parser import parse_fountain
from embeddings import process_embeddings
from summarizer import generate_scene_summaries, generate_story_summary, generate_character_profiles
from ollama_client import check_ollama_status

LOG_DIR = "log"

def save_to_log(filename, content):
    """Saves content to a log file."""
    if not os.path.exists(LOG_DIR):
        os.makedirs(LOG_DIR)
    
    filepath = os.path.join(LOG_DIR, filename)
    with open(filepath, "w", encoding="utf-8") as f:
        f.write(content)
    print(f"📁 Saved log: {filepath}")

def process_screenplay(file_path):
    """
    Orchestrates the screenplay analysis pipeline.
    
    Args:
        file_path (str): Path to the uploaded screenplay file.
        
    Returns:
        dict: Processed data including summaries and profiles.
    """
    if not check_ollama_status():
        return {"error": "Ollama server is offline. Please start it with 'ollama serve'."}
    
    try:
        with open(file_path, 'r', encoding='utf-8') as f:
            text = f.read()
    except Exception as e:
        return {"error": f"Failed to read file: {e}"}
        
    # 1. Parse
    scenes = parse_fountain(text)
    if not scenes:
        return {"error": "No scenes found. Please check file format."}
        
    # 2. Embeddings (with persistence)
    scenes_with_embeddings = process_embeddings(scenes)
    
    # 3. Scene Summaries
    print("Generating scene summaries...")
    summarized_scenes = generate_scene_summaries(scenes_with_embeddings)
    
    # Log Scene Summaries
    timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
    base_name = os.path.basename(file_path).replace('.txt', '')
    
    scene_log_content = f"SCENE SUMMARIES FOR {base_name}\n\n"
    for s in summarized_scenes:
        scene_log_content += f"SCENE {s['header']}\n{s['summary']}\n\n{'-'*40}\n\n"
    save_to_log(f"{base_name}_scenes_{timestamp}.txt", scene_log_content)

    # 4. Story Summary
    print("Generating story summary...")
    scene_summary_texts = [s['summary'] for s in summarized_scenes]
    story_summary = generate_story_summary(scene_summary_texts)
    
    # Log Story Summary
    save_to_log(f"{base_name}_story_{timestamp}.txt", story_summary)

    # 5. Character Summaries
    print("Generating character profiles...")
    character_profiles = generate_character_profiles(text)
    
    # Log Character Profiles
    char_log_content = f"CHARACTER PROFILES FOR {base_name}\n\n"
    for char, profile in character_profiles.items():
        char_log_content += f"👤 {char}\n{profile}\n\n{'-'*40}\n\n"
    save_to_log(f"{base_name}_characters_{timestamp}.txt", char_log_content)
    
    return {
        "scenes": summarized_scenes,
        "story_summary": story_summary,
        "character_profiles": character_profiles
    }

if __name__ == "__main__":
    # Simple CLI test
    import sys
    if len(sys.argv) > 1:
        result = process_screenplay(sys.argv[1])
        print(result)
    else:
        print("Usage: python main.py <path_to_screenplay.txt>")
