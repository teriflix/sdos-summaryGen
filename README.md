# Fountain Analysis Daemon

This project provides a backend daemon for analyzing Fountain screenplays. It parses Fountain files, manages sessions, and generates hierarchical summaries (scene, story, character) using the Ollama API.

## Features

- **Fountain Parsing**: Extracts scenes, dialogues, and metadata from `.fountain` files.
- **Session Management**: Persists analysis sessions to disk (`sessions/` directory), supporting resuming work across restarts.
- **Hierarchical Summarization**:
    - **Scene Summaries**: Generates summaries for individual scenes.
    - **Story Summary**: Aggregates scene summaries into a cohesive story overview.
    - **Character Profiles**: Analyzes character arcs based on dialogue and scene participation.
- **Caching**: Avoids redundant LLM calls by caching generated summaries.
- **REST API**: Exposes all functionality via a JSON-based HTTP API on port 8080.
- **Customizable Prompts**: Supports overriding the default system prompts for tailored analysis.

## Prerequisites

- **C++ Compiler**: GCC or Clang with C++17 support.
- **CMake**: Version 3.10 or higher.
- **Ollama**: Running locally with the desired model (default: `llama3.1:8b-instruct-q4_0`).
- **Dependencies**:
    - `cpp-httplib` (included/header-only)
    - `nlohmann/json` (included/header-only)
    - `libcurl`

## Building

1. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```

2. Configure and build:
   ```bash
   cmake ..
   make
   ```

## Usage

1. Start the daemon:
   ```bash
   ./fountain_tool
   ```
   The server listens on port `8080`.

2. Interact via API (see `apiUsage.md` for full documentation).

### Example: Start a Session

```bash
curl -X POST http://localhost:8080/session/start \
     -H "Content-Type: application/json" \
     -d '{
           "filepath": "/abs/path/to/script.fountain",
           "model_name": "llama3.1:8b-instruct-q4_0"
         }'
```

Response:
```json
{
  "session_id": "550e8400-e29b-41d4-a716-446655440000"
}
```

## Configuration

- **Port**: Set `PORT` environment variable (default: 8080).
- **Ollama URL**: Set `OLLAMA_BASE_URL` (default: http://127.0.0.1:11434).
- **Model**: Default model can be configured in `.env` or passed per request.

- Example .env
```json
{
  # .env file for Fountain Summarizer
# Configure your Ollama instance here

# Base URL for Ollama API
OLLAMA_BASE_URL=http://127.0.0.1:11434

# Model to use
# Examples: 
# - llama3.1:8b-instruct-q4_0 (Balanced)
# - llama3.2:3b (Faster, lower memory)
# - mistral (Good alternative)
OLLAMA_MODEL=llama3.1:8b-instruct-q4_0
}
```

## Project Structure

- `src/`: Source code.
  - `main.cpp`: Entry point and REST server.
  - `SessionManager`: Handles session persistence and lifecycle.
  - `FountainParser`: Parsing logic.
  - `Summarizer`: LLM interaction logic.
- `include/`: Header-only dependencies.
- `sessions/`: JSON storage for active sessions.
- `tests/`: specific testing scripts.

## License

LOL
