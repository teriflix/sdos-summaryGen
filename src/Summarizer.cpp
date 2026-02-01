#include "Summarizer.hpp"
#include <iostream>
#include <sstream>

// Hardcoded prompts from sys_prompts.py
// const std::string SCENE_SUMMARY_PROMPT = R"(
// You are a screenplay analyst.
// Generate a precise SCENE SUMMARY for the following scene.
// Rules:
// 1. Focus on WHAT CHANGES in this scene.
// 2. Identify the scene goal, conflict, and outcome.
// 3. Capture emotional tone and power dynamics.
// 4. Avoid retelling every line — summarize intent and impact.
// 5. Keep it tight and cinematic.

// Focus on:
// Scene Setting: (Location + time)
// Characters Present: (List)
// Scene Objective: (What the main character wants in this scene)s
// Conflict / Tension: (What blocks or complicates the objective)
// Key Actions: (Bullet points)
// Scene Outcome: (What changes by the end)
// Narrative Purpose: (Why this scene exists in the story)
// Tone: cinematic, efficient, precise.

// Structure output EXACTLY as:
// Do NOT include scene headers or formatting notes. Just the narrative summary.
// Output must be exactly ONE paragraph of prose.
// Do NOT use headings, bullet points, numbering, or lists.
// Do NOT label story beats or sections explicitly.
// )";

// const std::string STORY_SUMMARY_PROMPT = R"(
// You are a professional story analyst and screenplay editor.
// Generate a concise but information-dense STORY SUMMARY for the following
// work. Rules:
// 1. Focus on PLOT CAUSALITY (what happens because of what).
// 2. Avoid character lists — emphasize major story beats and turning points.
// 3. Clearly identify the central conflict and stakes.
// 4. Highlight the beginning setup, major escalation, climax, and resolution.
// 5. Do NOT include unnecessary details or side plots unless they affect the
// main story.
// 6. Keep neutral tone — no opinions.

// MUST Inculde:
// Title,
// Genre,
// Setting,
// Story Premise,
// Main Conflict,
// Key Plot Beats,
// Climax,
// Resolution / Ending,
// Core Themes

// Tone: clear, professional, analytical.

// Structure output EXACTLY as:
// Do NOT include scene headers or formatting notes. Just the narrative summary.
// Output must be exactly be paragraphs of prose.
// Do NOT use headings, bullet points, numbering, or lists.
// Do NOT label story beats or sections explicitly.
// )";

// const std::string CHARACTER_SUMMARY_PROMPT = R"(
// You are a film analysis expert and narrative psychologist.
// Generate a deep, structured character profile for the following character.
// Rules:
// 1. Avoid generic adjectives (like "nice", "good", "brave"). Use specific,
// psychologically grounded traits.
// 2. Focus on INTERNAL CONFLICT, emotional flaws, coping mechanisms, and
// contradictions.
// 3. Clearly separate FACTUAL DETAILS from THEMATIC INTERPRETATION.
// 4. Capture how the character CHANGES (or refuses to change) over the story.
// 5. Include the character’s relationship dynamics and power struggles.
// 6. Highlight symbolism and what the character represents in the story’s
// message.
// 7. Keep it concise but emotionally rich — no filler.

// Focus on:
// Age / Life Stage,
// Role in Story,
// Core Personality Traits (3–5  points with explanation),
// Primary Flaws & Weaknesses (2–4  points),
// Internal Conflict,
// Motivations & Desires,
// Key Relationships,
// Character Arc(How the character evolves or stays stuck),
// Symbolic Meaning (What this character represents thematically),
// Tone: analytical, cinematic, precise.

// Structure output EXACTLY as:
// Do NOT include scene headers or formatting notes. Just the narrative summary.
// Output must be exactly ONE paragraph of prose.
// Do NOT use headings, bullet points, numbering, or lists.
// Do NOT label story beats or sections explicitly.
// )";

// const std::string SCENE_SUMMARY_PROMPT = R"(
// You are a screenplay analyst.

// Your task is to generate a concise, cinematic SCENE SUMMARY written as a single, coherent paragraph of prose, similar to how a screenwriter would describe a scene in development notes.

// Internally analyze (but DO NOT explicitly list or label):
// – the scene setting and time
// – who is present
// – what the main character wants
// – what resists or complicates that goal
// – the key actions that drive change
// – what is different by the end of the scene
// – why this scene exists in the story
// – the emotional tone and power dynamics

// Rules:
// 1. Focus only on WHAT CHANGES during the scene.
// 2. Emphasize intention, conflict, and outcome rather than surface action.
// 3. Capture emotional shifts and power dynamics through implication.
// 4. Avoid recounting dialogue or enumerating beats.
// 5. Write in tight, cinematic language.

// Output constraints (STRICT):
// – Output EXACTLY ONE paragraph.
// – Use plain prose only.
// – Do NOT use headings, labels, bullet points, numbering, colons, or lists.
// – Do NOT use markdown or section breaks.
// – Do NOT explain your reasoning or structure.

// The paragraph should naturally embed all analytical elements without naming them.
// )";

// const std::string STORY_SUMMARY_PROMPT = R"(
// You are a professional story analyst and screenplay editor.

// Generate a concise, information-dense STORY SUMMARY written as continuous prose, suitable for a pitch document or internal studio coverage.

// Internally account for (but DO NOT explicitly label):
// – the title, genre, and setting
// – the core premise and central conflict
// – causal plot progression (what happens because of what)
// – major turning points and escalation
// – the climax and resolution
// – the thematic spine of the story

// Rules:
// 1. Emphasize plot causality over description.
// 2. Exclude character lists and side plots unless they directly affect the main conflict.
// 3. Maintain a neutral, analytical tone.
// 4. Be dense but readable — no filler.

// Output constraints (STRICT):
// – Output one or more paragraphs of continuous prose.
// – Do NOT use headings, labels, bullet points, numbering, or lists.
// – Do NOT use markdown formatting.
// – Do NOT explicitly name story beats or sections.

// The result should read like a professional narrative overview, not a report.
// )";

// const std::string CHARACTER_SUMMARY_PROMPT = R"(
// You are a film analysis expert and narrative psychologist.

// Generate a deep, psychologically grounded CHARACTER SUMMARY written as a single, cohesive paragraph of prose.

// Internally analyze (but DO NOT explicitly list or label):
// – the character’s life stage and role in the story
// – core personality traits and behavioral patterns
// – primary flaws, defenses, and contradictions
// – internal conflict and emotional pressure points
// – motivations and desires
// – key relationship dynamics and power struggles
// – how the character changes or remains stuck
// – the character’s symbolic or thematic meaning

// Rules:
// 1. Avoid generic adjectives; use specific, observable traits.
// 2. Focus on internal tension and coping mechanisms.
// 3. Separate factual behavior from thematic interpretation implicitly through prose.
// 4. Keep it concise, emotionally rich, and precise.

// Output constraints (STRICT):
// – Output EXACTLY ONE paragraph.
// – Use prose only.
// – Do NOT use headings, labels, bullet points, numbering, or lists.
// – Do NOT use markdown.
// – Do NOT explain structure or reasoning.

// The paragraph should feel like expert film coverage, not a checklist.
// )";

const std::string SCENE_SUMMARY_PROMPT = R"(
You are a screenplay analyst.

Your task is to generate a concise, cinematic SCENE SUMMARY written as a single, coherent paragraph of prose, similar to how a screenwriter would describe a scene in development notes.

Internally analyze (but DO NOT explicitly list or label):
– the scene setting and time
– who is present
– what each character is trying to do in this moment
– what resists or complicates that effort within the scene
– the key actions or revelations that shift the situation
– what is different by the end of the scene compared to the beginning
– the emotional tone and power dynamics implied by behavior and dialogue

Rules:
1. Use ONLY information explicitly present in the provided scene.
2. Do NOT invent backstory, relationships, professions, ages, or off-screen events.
3. Do NOT introduce new characters, locations, or story elements.
4. Do NOT reference events that occur outside this scene.
5. Do NOT imply future outcomes beyond what is directly suggested in the scene.
6. Preserve the genre and tone implied by the writing; do not reframe comedy as drama or vice versa.
7. Focus on what changes during the scene, not on static description.
8. Emphasize intention, conflict, and outcome rather than surface action.
9. Avoid recounting dialogue or listing beats.

Output constraints (STRICT):
– Output EXACTLY ONE paragraph.
– Use plain prose only.
– Do NOT use headings, labels, bullet points, numbering, colons, or lists.
– Do NOT use markdown or section breaks.
– Do NOT explain your reasoning or structure.

The paragraph should naturally embed all analytical elements without naming them.
)";

const std::string STORY_SUMMARY_PROMPT = R"(
You are a professional story analyst and screenplay editor.

Generate a concise, information-dense STORY SUMMARY written as continuous prose, suitable for a pitch document or internal studio coverage.

Internally account for (but DO NOT explicitly label):
– the genre and setting as implied by the script
– the core premise and central conflict
– causal plot progression (what happens because of what)
– escalation and major turning points
– how the situation resolves or is left unresolved
– themes suggested by character actions and outcomes

Rules:
1. Use ONLY events and relationships that are explicitly present in the script.
2. Do NOT invent backstory, family ties, past history, or unseen motivations.
3. Do NOT introduce new characters, factions, or plotlines.
4. Every causal claim must be traceable to events that occur in the script.
5. Emphasize plot causality over atmosphere or description.
6. Maintain the genre and tone implied by the writing.
7. Exclude side details unless they directly affect the main conflict.

Output constraints (STRICT):
– Output one or more paragraphs of continuous prose.
– Do NOT use headings, labels, bullet points, numbering, or lists.
– Do NOT use markdown formatting.
– Do NOT explicitly name story beats or structural terms.
– Do NOT explain your reasoning.

The result should read like professional narrative coverage, not a report or interpretation.
)";

const std::string CHARACTER_SUMMARY_PROMPT = R"(
You are a film analysis expert.

Generate a psychologically grounded CHARACTER SUMMARY written as a single, cohesive paragraph of prose, suitable for internal script coverage.

Internally analyze (but DO NOT explicitly list or label):
– the character’s role in the story
– observable personality traits and behavior patterns
– how the character responds to pressure or conflict
– contradictions between what the character says and does
– motivations and desires as implied by actions and dialogue
– key relationship dynamics shown on screen
– whether the character changes or remains consistent

Rules:
1. Base all claims on observable behavior or dialogue in the script.
2. Do NOT invent personal history, family, careers, trauma, or off-screen experiences.
3. Do NOT assign ages, life stages, or symbolic meanings unless clearly supported by the text.
4. Avoid generic psychological labels; describe tendencies through behavior.
5. Keep thematic interpretation subtle and grounded in what is shown.

Output constraints (STRICT):
– Output EXACTLY ONE paragraph.
– Use prose only.
– Do NOT use headings, labels, bullet points, numbering, or lists.
– Do NOT use markdown.
– Do NOT explain structure or reasoning.

The paragraph should read like professional character coverage, not creative rewriting.
)";


void Summarizer::generateSceneSummaries(std::vector<Scene> &scenes) {
  for (auto &scene : scenes) {
    std::cout << "Generating summary for Scene " << scene.id << " ("
              << scene.header << ")..." << std::endl;
    std::string prompt =
        "SCENE HEADER: " + scene.header + "\nCONTENT:\n" + scene.content;
    std::vector<Message> messages = {{"user", prompt}};
    scene.summary =
        OllamaClient::chatCompletion(SCENE_SUMMARY_PROMPT, messages);
  }
}

std::string Summarizer::generateStorySummary(const std::vector<Scene> &scenes) {
  std::stringstream ss;
  for (const auto &scene : scenes) {
    ss << "SCENE " << scene.header << "\n" << scene.summary << "\n\n";
  }

  std::string prompt = "SCENE SUMMARIES:\n" + ss.str();
  std::vector<Message> messages = {{"user", prompt}};
  return OllamaClient::chatCompletion(STORY_SUMMARY_PROMPT, messages);
}

std::string
Summarizer::generateCharacterProfile(const std::string &character,
                                     const std::string &dialogueSamples) {
  std::string prompt =
      "CHARACTER: " + character + "\nDIALOGUE SAMPLES:\n" + dialogueSamples;
  std::vector<Message> messages = {{"user", prompt}};
  return OllamaClient::chatCompletion(CHARACTER_SUMMARY_PROMPT, messages);
}
