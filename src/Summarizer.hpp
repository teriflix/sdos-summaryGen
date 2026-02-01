#ifndef SUMMARIZER_HPP
#define SUMMARIZER_HPP

#include "FountainParser.hpp"
#include "OllamaClient.hpp"
#include <string>
#include <vector>

class Summarizer {
public:
  static void generateSceneSummaries(std::vector<Scene> &scenes);
  static std::string generateStorySummary(const std::vector<Scene> &scenes);
  static std::string
  generateCharacterProfile(const std::string &character,
                           const std::string &dialogueSamples);
};

#endif // SUMMARIZER_HPP
