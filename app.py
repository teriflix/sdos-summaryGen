import streamlit as st
import os
from main import process_screenplay
from ollama_client import check_ollama_status

st.set_page_config(page_title="Screenplay Analysis Tool", layout="wide")

st.title("🎬 Screenplay Analysis Tool")

# Check Ollama Status
if not check_ollama_status():
    st.error("❌ Ollama server is offline. Please start it with `ollama serve` and refresh this page.")
    st.stop()

# Sidebar
st.sidebar.header("Upload Screenplay")
uploaded_file = st.sidebar.file_uploader("Choose a .txt file (Fountain/OSF)", type="txt")

if uploaded_file is not None:
    # Save file temporarily
    save_path = os.path.join("data", "uploads", uploaded_file.name)
    with open(save_path, "wb") as f:
        f.write(uploaded_file.getbuffer())
    
    st.sidebar.success(f"Uploaded: {uploaded_file.name}")
    
    if st.sidebar.button("Process Screenplay"):
        with st.spinner("Processing... This may take a while depending on script length."):
            result = process_screenplay(save_path)
            
            if "error" in result:
                st.error(result["error"])
            else:
                st.session_state['analysis_result'] = result
                st.success("Processing Complete!")

# Display Results
if 'analysis_result' in st.session_state:
    result = st.session_state['analysis_result']
    
    tab1, tab2, tab3 = st.tabs(["📝 Scene Summaries", "📖 Story Summary", "👥 Character Profiles"])
    
    with tab1:
        st.header("Scene Summaries")
        for scene in result["scenes"]:
            with st.expander(f"{scene['header']}"):
                st.markdown(f"**Summary:** {scene['summary']}")
                # Optionally show original content
                # st.text(scene['content'])
                
    with tab2:
        st.header("Full Story Summary")
        st.markdown(result["story_summary"])
        
    with tab3:
        st.header("Character Profiles")
        for char, profile in result["character_profiles"].items():
            with st.expander(f"👤 {char}"):
                st.markdown(profile)
else:
    st.info("Upload a screenplay to begin analysis.")

# Basic styling
st.markdown("""
<style>
    .stExpander {
        border: 1px solid #ddd;
        border-radius: 5px;
    }
</style>
""", unsafe_allow_html=True)
