# AI-Powered Fountain Screenplay Summarizer

A high-performance C++ CLI tool that parses Fountain-formatted screenplays and uses local LLMs (via Ollama) to generate scene summaries, story synopses, and character profiles.

## Features

- **Protocol-Compliant Parsing**: Custom C++ implementation of the Fountain screenplay syntax.
- **Local AI Integration**: Connects to [Ollama](https://ollama.com/) running locally.
- **Configurable**: Support for `.env` file to configure model and API URL.
- **Logging**: Automatically saves generated reports to the `log/` directory.
- **Granular Analysis**:
    - **Scene Summary**: Summarize individual scenes by header/location.
    - **Story Summary**: Aggregate scene summaries into a cohesive plot synopsis.
    - **Character Profiling**: Analyze dialogue and interactions to build psychological profiles.

## Prerequisites

- **C++ Compiler**: C++17 compliant (GCC, Clang, MSVC).
- **CMake**: Version 3.10 or higher.
- **libcurl**: Used for HTTP requests.
- **Ollama**: Must be installed and running.

## Build Instructions

1.  **Clone the repository**:
    ```bash
    git clone <repo_url>
    cd <repo_name>
    ```

2.  **Create a build directory**:
    ```bash
    mkdir build
    cd build
    ```

3.  **Compile**:
    ```bash
    cmake ..
    make
    ```

## Configuration (.env)

Create a `.env` file in the same directory as the executable to configure Ollama. A sample `.env` is created for you.

```ini
OLLAMA_BASE_URL=http://127.0.0.1:11434
# Choose a model that fits your hardware (e.g., llama3.2:1b for low VRAM, llama3.1:8b for better quality)
OLLAMA_MODEL=llama3.1:8b-instruct-q4_0
```

## Usage

Ensure Ollama is running:
```bash
ollama serve
```

### 1. Generate Full Story & Character Summary (Default)
Parses the entire script, generates a story synopsis, and creates psychological profiles for the top 5 characters (detected by dialogue count).
logs are saved to `log/`.
```bash
./fountain_tool path/to/script.fountain
```

### 2. Summarize a Specific Scene
Finds a scene matching the provided header fragment.
```bash
./fountain_tool path/to/script.fountain --scene "INT. COFFEE SHOP"
```

### 3. Generate Character Profile
```bash
./fountain_tool path/to/script.fountain --character "MARK"
```

## Logs

Generated summaries and profiles are automatically saved to the `log/` folder with timestamps, e.g.:
- `log/script_20231027_123045_story_full.txt` (Contains story, character profiles, and scene breakdowns)
- `log/script_20231027_123045_character_MARK.txt`

## Dependencies

- [nlohmann/json](https://github.com/nlohmann/json) (Included in `include/`)

## License

MIT
