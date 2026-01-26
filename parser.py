import re

def parse_fountain(text):
    """
    Parses a Fountain formatted screenplay text into scenes.
    
    Args:
        text (str): The full text of the screenplay.
        
    Returns:
        list: A list of dictionaries, each representing a scene.
              Format: {'id': int, 'header': str, 'content': str}
    """
    lines = text.split('\n')
    scenes = []
    current_scene = {'id': 0, 'header': 'START', 'content': []}
    scene_count = 0
    
    # Regex for Scene Headings (Sluglines)
    # Starts with INT, EXT, EST, I/E, int, ext, est, i/e followed by dot or space
    scene_heading_regex = re.compile(r'^(?:\.?(?:INT|EXT|EST|I\/E)[.\s])', re.IGNORECASE)

    for line in lines:
        stripped_line = line.strip()
        
        # Check if line is a Scene Heading
        if scene_heading_regex.match(stripped_line):
            # Save previous scene if it has content
            if current_scene['content']:
                current_scene['content'] = '\n'.join(current_scene['content']).strip()
                scenes.append(current_scene)
            
            # Start new scene
            scene_count += 1
            current_scene = {
                'id': scene_count,
                'header': stripped_line,
                'content': []
            }
        else:
            current_scene['content'].append(line)
            
    # Add the last scene
    if current_scene['content']:
        current_scene['content'] = '\n'.join(current_scene['content']).strip()
        scenes.append(current_scene)
        
    return scenes
