import os
import json
import pickle
import hashlib
from ollama_client import generate_embedding

EMBEDDINGS_DIR = "data/embeddings"

def get_file_hash(text):
    """Generates a SHA256 hash for the given text."""
    return hashlib.sha256(text.encode('utf-8')).hexdigest()

def save_embeddings(file_hash, data):
    """Saves embeddings data to a pickle file."""
    if not os.path.exists(EMBEDDINGS_DIR):
        os.makedirs(EMBEDDINGS_DIR)
    
    file_path = os.path.join(EMBEDDINGS_DIR, f"{file_hash}.pkl")
    with open(file_path, 'wb') as f:
        pickle.dump(data, f)

def load_embeddings(file_hash):
    """Loads embeddings data from a pickle file if it exists."""
    file_path = os.path.join(EMBEDDINGS_DIR, f"{file_hash}.pkl")
    if os.path.exists(file_path):
        with open(file_path, 'rb') as f:
            return pickle.load(f)
    return None

def process_embeddings(scenes):
    """
    Generates embeddings for a list of scenes.
    
    Args:
        scenes (list): List of scene dictionaries.
        
    Returns:
        list: List of scenes with an added 'embedding' key.
    """
    # Create a unique hash for the collection of scenes content to check availability
    full_text = "".join([s['header'] + s['content'] for s in scenes])
    file_hash = get_file_hash(full_text)
    
    cached_data = load_embeddings(file_hash)
    if cached_data:
        return cached_data
    
    processed_scenes = []
    for scene in scenes:
        text_to_embed = f"{scene['header']}\n{scene['content']}"
        embedding = generate_embedding(text_to_embed)
        
        scene_data = scene.copy()
        scene_data['embedding'] = embedding
        processed_scenes.append(scene_data)
        
    save_embeddings(file_hash, processed_scenes)
    return processed_scenes
