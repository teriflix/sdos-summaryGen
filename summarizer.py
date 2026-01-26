from ollama_client import chat_completion
from sys_prompts import SCENE_SUMMARY_PROMPT, STORY_SUMMARY_PROMPT, CHARACTER_SUMMARY_PROMPT

def generate_scene_summaries(scenes):
    """
    Generates summaries for each scene.
    
    Args:
        scenes (list): List of scene dictionaries.
        
    Returns:
        list: List of scene dictionaries with an added 'summary' key.
    """
    summarized_scenes = []
    for scene in scenes:
        prompt = f"SCENE HEADER: {scene['header']}\nCONTENT:\n{scene['content']}"
        messages = [{"role": "user", "content": prompt}]
        summary = chat_completion(SCENE_SUMMARY_PROMPT, messages)
        
        scene_data = scene.copy()
        scene_data['summary'] = summary
        summarized_scenes.append(scene_data)
        
    return summarized_scenes

def generate_story_summary(scene_summaries):
    """
    Generates a full story summary from scene summaries.
    
    Args:
        scene_summaries (list): List of strings containing scene summaries.
        
    Returns:
        str: The full story summary.
    """
    joined_summaries = "\n\n".join(scene_summaries)
    prompt = f"SCENE SUMMARIES:\n{joined_summaries}"
    messages = [{"role": "user", "content": prompt}]
    return chat_completion(STORY_SUMMARY_PROMPT, messages)

def generate_character_profiles(full_text):
    """
    Generates character profiles from the full script text.
    (Note: Passing full text might exceed context for very long scripts, 
    but for this MVP we will try to pass a significant chunk or assume the model handles it,
    or we could strictly parse out dialogue per character if needed. 
    For better accuracy with standard context windows, we might need to be smarter here,
    but let's start with a naive approach or chunks if it's too big.)
    
    For now, to save context, we might just pass the scene summaries + list of characters? 
    Or we can implement a logic to extract top characters first.
    
    Let's stick to the prompt's request: "Process script and extract... Top 5 characters".
    We might need to count dialogue lines first to identify top 5.
    """
    
    # Simple heuristic to find top characters (by line count) would be better than sending full text blind.
    # But for the "Generative" part, we need text. 
    # Let's try to send a compressed version or just the dialogue of top characters if possible.
    
    # 1. Identify top 5 characters
    lines = full_text.split('\n')
    character_counts = {}
    current_character = None
    
    # Very basic heuristic for character names (all caps, not scene header)
    import re
    # Matches lines that are ALL CAPS, widely indented (usually), but standard simple text files might not have indent.
    # Fountain: Character name is uppercase, single line, followed by dialogue.
    
    for i, line in enumerate(lines):
        line = line.strip()
        if line.isupper() and len(line) > 0 and not line.startswith(("INT.", "EXT.", "EST.")):
            # Check if next line is not empty (dialogue) - simple heuristic
            if i + 1 < len(lines) and lines[i+1].strip():
                char_name = line.split('(')[0].strip() # Remove (V.O.) etc
                character_counts[char_name] = character_counts.get(char_name, 0) + 1

    top_5_chars = sorted(character_counts.items(), key=lambda x: x[1], reverse=True)[:5]
    top_5_names = [c[0] for c in top_5_chars]
    
    # 2. Extract dialogue/context for these characters? 
    # Sending full text might be too heavy. Let's try sending the *story summary* and the names, 
    # asking the LLM to hallucinate/infer based on the summary? No, that's bad.
    # Better: Send the first N lines or chunks? 
    # best approach for this task size: send the generated *scene summaries* to the model 
    # and ask it to profile the characters based on the plot summary. 
    # It won't have tone/dialogue style but it will have role/arc.
    # ALTHOUGH the prompt says "Based on the provided dialogue...". 
    # So we should probably extract some dialogue samples.
    
    # Let's aggregate up to 2000 chars of dialogue per character.
    char_dialogues = {name: [] for name in top_5_names}
    
    for i, line in enumerate(lines):
        line = line.strip()
        cleaned_line = line.split('(')[0].strip()
        if cleaned_line in top_5_names:
             if i + 1 < len(lines) and lines[i+1].strip():
                 char_dialogues[cleaned_line].append(lines[i+1].strip())
    
    profiles = {}
    for char, dialogues in char_dialogues.items():
        sample_dialogue = "\n".join(dialogues[:20]) # First 20 lines of dialogue
        prompt = f"CHARACTER: {char}\nDIALOGUE SAMPLES:\n{sample_dialogue}"
        messages = [{"role": "user", "content": prompt}]
        profile = chat_completion(CHARACTER_SUMMARY_PROMPT, messages)
        profiles[char] = profile
        
    return profiles
