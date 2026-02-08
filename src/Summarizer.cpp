#include "Summarizer.hpp"
#include <iostream>
#include <sstream>

const std::string SCENE_SUMMARY_PROMPT = R"(
You are a screenplay analyst.
Your task is to generate a concise, cinematic SCENE SUMMARY written as a single, coherent paragraph of prose.
Internally analyze:
– the scene setting and time
– who is present
– key actions and conflict
– emotional tone

Rules:
1. Use ONLY information explicitly present in the provided scene.
2. Focus on what changes during the scene.
3. Emphasize intention, conflict, and outcome rather than surface action.

Output constraints:
– Output EXACTLY ONE paragraph.
– Use plain prose only.
)";

const std::string STORY_SUMMARY_PROMPT = R"(
You are a professional story analyst.
Generate a concise, information-dense STORY SUMMARY written as continuous prose.
Internally account for:
– the core premise and central conflict
– causal plot progression
– major turning points
– resolution

Rules:
1. Use ONLY events present in the script summaries.
2. Emphasize plot causality.

Output constraints:
– Output one or more paragraphs of continuous prose.
)";

const std::string CHARACTER_SUMMARY_PROMPT = R"(
You are a film analysis expert.
Generate a psychologically grounded CHARACTER SUMMARY written as a single, cohesive paragraph of prose.
Internally analyze:
– the character’s role
– personality traits and behavior
– motivations and desires
– key relationships

Rules:
1. Base all claims on observable behavior or dialogue.
2. Avoid generic psychological labels.

Output constraints:
– Output EXACTLY ONE paragraph.
– Use prose only.
)";

std::string Summarizer::generateSceneSummary(Session &session, int sceneId,
                                             const std::string &model,
                                             const std::string &systemPrompt) {
  Scene *targetScene = nullptr;
  for (auto &scene : session.scenes) {
    if (scene.id == sceneId) {
      targetScene = &scene;
      break;
    }
  }

  if (!targetScene) {
    return "Error: Scene not found.";
  }

  if (!targetScene->summary.empty()) {
    return targetScene->summary;
  }

  std::string usedModel = model.empty() ? session.config.modelName : model;
  // Fallback if model name is empty
  if (usedModel.empty())
    usedModel = "llama3.2";

  std::string userPrompt = "SCENE HEADER: " + targetScene->header +
                           "\nCONTENT:\n" + targetScene->content;

  std::string promptToUse =
      systemPrompt.empty() ? SCENE_SUMMARY_PROMPT : systemPrompt;

  std::string summary = OllamaClient::chatCompletion(
      promptToUse, {{"user", userPrompt}}, usedModel);

  targetScene->summary = summary;
  SessionManager::getInstance().saveSession(session.id);

  return summary;
}

std::string Summarizer::generateStorySummary(Session &session,
                                             const std::string &model,
                                             const std::string &systemPrompt) {
  if (!session.storySummary.empty()) {
    return session.storySummary;
  }

  std::stringstream combinedSummaries;
  int count = 0;
  for (const auto &scene : session.scenes) {
    if (!scene.summary.empty()) {
      combinedSummaries << "Scene " << scene.id << ": " << scene.header << "\n"
                        << scene.summary << "\n\n";
      count++;
    }
  }

  if (count == 0) {
    return "Error: No scene summaries available.";
  }

  if (count < session.scenes.size() / 3) {
    return "Error: Insufficient scene summaries. Need at least 1/3 of scenes "
           "summarized.";
  }

  std::string usedModel = model.empty() ? session.config.modelName : model;
  if (usedModel.empty())
    usedModel = "llama3.2";

  std::string userPrompt =
      "Here are summaries of scenes. Create a cohesive story summary.\n\n" +
      combinedSummaries.str();

  std::string promptToUse =
      systemPrompt.empty() ? STORY_SUMMARY_PROMPT : systemPrompt;

  std::string summary = OllamaClient::chatCompletion(
      promptToUse, {{"user", userPrompt}}, usedModel);

  session.storySummary = summary;
  SessionManager::getInstance().saveSession(session.id);

  return summary;
}

std::string Summarizer::generateCharacterSummary(
    Session &session, const std::string &characterName,
    const std::string &model, const std::string &systemPrompt) {
  if (session.characters.find(characterName) == session.characters.end()) {
    return "Error: Character not found.";
  }

  CharacterProfile &profile = session.characters[characterName];

  if (!profile.summary.empty()) {
    return profile.summary;
  }

  std::stringstream dialogues;
  for (int sceneId : profile.sceneIds) {
    for (const auto &scene : session.scenes) {
      if (scene.id == sceneId) {
        if (!scene.summary.empty()) {
          dialogues << "[Scene " << scene.id << " Context]: " << scene.summary
                    << "\n";
        }
        for (const auto &d : scene.dialogues) {
          if (d.character == characterName) {
            dialogues << d.character << ": " << d.text << "\n";
          }
        }
        dialogues << "\n";
        break;
      }
    }
  }

  std::string usedModel = model.empty() ? session.config.modelName : model;
  if (usedModel.empty())
    usedModel = "llama3.2";

  std::stringstream prompt;
  prompt << "Analyze the character '" << characterName
         << "' based on the following dialogues and scene contexts.\n";
  if (!session.storySummary.empty()) {
    prompt << "Story Context: " << session.storySummary << "\n\n";
  }
  prompt << "Character Data:\n" << dialogues.str();

  std::string promptToUse =
      systemPrompt.empty() ? CHARACTER_SUMMARY_PROMPT : systemPrompt;

  std::string summary = OllamaClient::chatCompletion(
      promptToUse, {{"user", prompt.str()}}, usedModel);

  profile.summary = summary;
  SessionManager::getInstance().saveSession(session.id);

  return summary;
}
