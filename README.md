#  Screenplay Analysis Tool

A local-first, privacy-focused tool for analyzing screenplays using LLMs. This application ingests screenplays (in `.txt`, Fountain, or OSF format), parses them into scenes, and uses local AI models to generate detailed summaries, plot outlines, and character profiles.

##  Features

-   **Scene-by-Scene Analysis**: Automatically breaks down scripts and summarizes each scene, identifying key actions, conflicts, and outcomes.
-   **Story Plotting**: Synthesizes scene summaries into a cohesive high-level story synopsis, tracking the narrative arc.
-   **Character Profiling**: extracts deep psychological profiles for major characters, including traits, flaws, motivations, and relationships.
-   **Privacy-First**: Runs entirely locally using [Ollama](https://ollama.com/), ensuring your creative work never leaves your machine.
-   **Dual Interface**:
    -   **Web UI**: Interactive Streamlit dashboard for easy uploading and viewing of analysis.
    -   **CLI**: Command-line interface for batch processing or piping results.

##  Prerequisites

-   **Python 3.8+**
-   **[Ollama](https://ollama.com/)**: Must be installed and running locally.

##  Installation

1.  **Clone the repository:**
    ```bash
    git clone <repository-url>
    cd <repository-url>
    ```

2.  **Install Python dependencies:**
    ```bash
    pip install -r requirements.txt
    ```

3.  **Set up Ollama:**
    Ensure Ollama is running (`ollama serve`). Then, pull the required models:
    ```bash
    ollama pull llama3.1:8b-instruct-q4_0
    ollama pull nomic-embed-text:latest
    ```
    *Note: You can configure different models in the `.env` file.*

4.  **Configuration (Optional):**
    Create a `.env` file if you wish to override defaults:
    ```env
    OLLAMA_BASE_URL=http://127.0.0.1:11434
    CHAT_MODEL=llama3.1:8b-instruct-q4_0
    EMBED_MODEL=nomic-embed-text:latest
    ```

## 📖 Usage

### Web Interface (Recommended)

Run the Streamlit app:
```bash
streamlit run app.py
```
This will open a browser window where you can upload your screenplay file and view the interactive results.

### Command Line Interface

Process a script directly from the terminal:
```bash
python main.py path/to/script.txt
```
This will print the analysis to stdout and save detailed logs to the `log/` directory.

##  Project Structure

-   `app.py`: Streamlit web application.
-   `main.py`: Core logic and CLI entry point.
-   `ollama_client.py`: Handles communication with the local Ollama instance.
-   `parser.py`: Logic for parsing Fountain/text screenplays.
-   `summarizer.py`: Functions for generating specific types of summaries.
-   `sys_prompts.py`: System prompts used to guide the LLM's analysis.
-   `embeddings.py`: Handles vector embedding generation (for future RAG capabilities).
-   `log/`: Directory where analysis outputs are saved.
