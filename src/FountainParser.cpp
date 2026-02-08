#include "FountainParser.hpp"
#include "Utils.hpp"
#include <iostream>
#include <regex>
#include <sstream>

std::vector<Scene> FountainParser::parse(const std::string &text) {
  std::vector<Scene> scenes;
  std::stringstream ss(text);
  std::string line;

  // Regex for Scene Headings
  std::regex sceneHeadingRegex(R"(^(\.?(?:INT|EXT|EST|I\/E)[\.\s]).*)",
                               std::regex_constants::icase |
                                   std::regex_constants::optimize);

  // Regex for Character Names (Simple heuristic: All CAPS, not a scene
  // header, not transitions)
  // We'll rely more on indentation/fountain rules if possible, but for plain
  // text/loose fountain:
  // Strictly speaking, Fountain requires uppercase.
  std::regex characterRegex(R"(^[\s]*[A-Z][A-Z0-9\s\.'\(\)]+$)");
  // Parentheticals
  std::regex parentheticalRegex(R"(^[\s]*\(.*\)$)");

  Scene currentScene;
  currentScene.id = 0;
  currentScene.header = "START";
  currentScene.startLine = 1;

  int lineNumber = 0;
  int sceneStartLine = 1;
  int sceneCount = 0;

  std::string currentContent;
  std::string lastCharacter;
  bool expectingDialogue = false;

  while (std::getline(ss, line)) {
    lineNumber++;
    std::string strippedLine = Utils::trim(line);

    if (strippedLine.empty()) {
      currentContent += "\n"; // Preserve spacing
      expectingDialogue = false;
      continue;
    }

    // Check for Scene Header
    if (std::regex_match(strippedLine, sceneHeadingRegex)) {
      // Save previous scene
      if (!currentContent.empty()) {
        currentScene.content = Utils::trim(currentContent);
        currentScene.endLine = lineNumber - 1;
        scenes.push_back(currentScene);
      }

      // Start new scene
      sceneCount++;
      currentScene = Scene(); // Reset
      currentScene.id = sceneCount;
      currentScene.header = strippedLine;
      currentScene.startLine = lineNumber;
      currentScene.content = ""; // Header is not part of content body usually,
                                 // or we can include it.
      currentContent = "";
      lastCharacter = "";
      expectingDialogue = false;
    } else {
      // Check for Character Name (for dialogue)
      // Heuristic: Uppercase, not too long, indented or not (Fountain allows
      // forced @)
      bool isCharacter = false;
      // We assume character names are not scene headers (already checked).
      // If line is all caps and reasonably short.
      if (!expectingDialogue && strippedLine.length() < 50 &&
          std::regex_match(strippedLine, characterRegex)) {
        // Exclude transitions like "CUT TO:" usually they are right aligned or
        // all caps.
        // For simplicity, treat as character if it looks like one.
        // Exclude "THE END"
        if (strippedLine != "THE END") {
          lastCharacter = strippedLine;
          expectingDialogue = true;
          isCharacter = true;
        }
      }

      // If we are expecting dialogue and this line is not empty and not a
      // parenthetical
      if (expectingDialogue && !isCharacter) {
        if (std::regex_match(strippedLine, parentheticalRegex)) {
          // It's a parenthetical, just add to content, still expecting dialogue
          // technically, or extension of character
        } else {
          // It's dialogue
          Dialogue d;
          d.character = lastCharacter;
          d.text = strippedLine;
          d.lineNumber = lineNumber;
          currentScene.dialogues.push_back(d);
          // Don't reset expectingDialogue immediately if we want block
          // dialogue, but usually one line check is enough for basic mapping.
          // Actually, in Fountain, dialogue can be multiple lines.
          // We'll treat subsequent non-empty lines as potential dialogue if
          // they follow.
        }
      }

      currentContent += line + "\n";
    }
  }

  // Add the last scene
  if (!currentContent.empty()) {
    currentScene.content = Utils::trim(currentContent);
    currentScene.endLine = lineNumber;
    scenes.push_back(currentScene);
  }

  return scenes;
}
