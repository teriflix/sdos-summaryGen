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
  // Starts with .? then (INT|EXT|EST|I/E) then dot or space
  // Equivalent to py: r'^(?:\.?(?:INT|EXT|EST|I\/E)[.\s])'
  std::regex sceneHeadingRegex(R"(^(\.?(?:INT|EXT|EST|I\/E)[\.\s]).*)",
                               std::regex_constants::icase |
                                   std::regex_constants::optimize);

  Scene currentScene;
  currentScene.id = 0;
  currentScene.header = "START";

  int sceneCount = 0;
  std::string currentContent;

  while (std::getline(ss, line)) {
    std::string strippedLine = Utils::trim(line);
    if (strippedLine.empty()) {
      // Keep empty lines in content to preserve spacing if needed,
      // or we could skip them. Let's keep them but maybe just one newline.
      // For now, simple append.
      if (!currentContent.empty())
        currentContent += "\n";
      continue;
    }

    if (std::regex_match(strippedLine, sceneHeadingRegex)) {
      // Save previous scene if it has content
      if (!currentContent.empty()) {
        currentScene.content = Utils::trim(currentContent);
        scenes.push_back(currentScene);
      }

      // Start new scene
      sceneCount++;
      currentScene.id = sceneCount;
      currentScene.header = strippedLine;
      currentScene.content = "";
      currentScene.summary = "";
      currentContent = "";
    } else {
      if (!currentContent.empty())
        currentContent += "\n";
      currentContent +=
          line; // Use original line to preserve internal indentation if any
    }
  }

  // Add the last scene
  if (!currentContent.empty()) {
    currentScene.content = Utils::trim(currentContent);
    scenes.push_back(currentScene);
  }

  return scenes;
}
