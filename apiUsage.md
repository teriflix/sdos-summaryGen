# Backend Daemon API Usage

The backend daemon listens on port `8080` by default. All requests and responses are in JSON format.

## Base URL
`http://localhost:8080`

## Endpoints

### 1. Start Session
Initialize a new session with a screenplay file and configuration.

- **Endpoint**: `/session/start`
- **Method**: `POST`
- **Headers**: `Content-Type: application/json`
- **Body**:
  ```json
  {
    "filepath": "/absolute/path/to/script.fountain",
    "context_window": 8192,
    "model_name": "llama3.1:8b-instruct-q4_0"
  }
  ```
- **Response**:
  ```json
  {
    "session_id": "uuid-string"
  }
  ```

### 2. Generate Scene Summary
Generate or retrieve a cached summary for a specific scene.

- **Endpoint**: `/summary/scene`
- **Method**: `POST`
- **Body**:
  ```json
  {
    "session_id": "uuid-string",
    "scene_id": 1,
    "model": "optional-model-override",
    "system_prompt": "Optional system prompt override"
  }
  ```
- **Response**:
  ```json
  {
    "summary": "This scene depicts..."
  }
  ```

### 3. Generate Story Summary
Generate or retrieve a cached summary for the entire story. Requires sufficient scene summaries to be generated first (approx. 1/3 of total scenes).

- **Endpoint**: `/summary/story`
- **Method**: `POST`
- **Body**:
  ```json
  {
    "session_id": "uuid-string",
    "model": "optional-model-override",
    "system_prompt": "Optional system prompt override"
  }
  ```
- **Response**:
  ```json
  {
    "summary": "The story follows..."
  }
  ```

### 4. Generate Character Summary
Generate or retrieve a cached profile for a character.

- **Endpoint**: `/summary/character`
- **Method**: `POST`
- **Body**:
  ```json
  {
    "session_id": "uuid-string",
    "character_name": "HERO",
    "model": "optional-model-override",
    "system_prompt": "Optional system prompt override"
  }
  ```
- **Response**:
  ```json
  {
    "summary": "HERO is a complex character..."
  }
  ```

### 5. Get Session Metadata
Retrieve full session state, including parsed scenes, characters, and current summaries.

- **Endpoint**: `/session/{session_id}`
- **Method**: `GET`
- **Response**:
  ```json
  {
    "id": "uuid-string",
    "filepath": "/path/to/file",
    "config": { ... },
    "scenes": [
      {
        "id": 1,
        "header": "INT. ROOM - DAY",
        "summary": "..."
      }
    ],
    "characters": {
      "HERO": {
        "dialogueCount": 42,
        "summary": "..."
      }
    },
    "storySummary": "..."
  }
  ```

### 6. Delete Session
Remove a session from memory and disk storage.

- **Endpoint**: `/session/{session_id}`
- **Method**: `DELETE`
- **Response**:
  ```json
  {
    "status": "deleted"
  }
  ```

## Error Handling
Errors are returned with appropriate HTTP status codes (e.g., 400, 404, 500) and a JSON body:
```json
{
  "error": "Error message description"
}
```
