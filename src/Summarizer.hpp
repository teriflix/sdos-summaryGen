#ifndef SUMMARIZER_HPP
#define SUMMARIZER_HPP

#include "OllamaClient.hpp"
#include "SessionManager.hpp"
#include <string>
#include <vector>

class Summarizer {
public:
  static std::string generateSceneSummary(Session &session, int sceneId,
                                          const std::string &model = "",
                                          const std::string &systemPrompt = "");
  static std::string generateStorySummary(Session &session,
                                          const std::string &model = "",
                                          const std::string &systemPrompt = "");
  static std::string
  generateCharacterSummary(Session &session, const std::string &characterName,
                           const std::string &model = "",
                           const std::string &systemPrompt = "");
};

#endif // SUMMARIZER_HPP
