#ifndef FOUNTAIN_PARSER_HPP
#define FOUNTAIN_PARSER_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

struct Dialogue {
  std::string character;
  std::string text;
  int lineNumber;

  json toJson() const {
    return {{"character", character}, {"text", text}, {"lineNumber", lineNumber}};
  }
};

struct Scene {
  int id;
  std::string header;
  std::string content;
  int startLine;
  int endLine;
  std::vector<Dialogue> dialogues;
  std::string summary; // Will be filled later

  json toJson() const {
    json jDialogues = json::array();
    for (const auto &d : dialogues) {
      jDialogues.push_back(d.toJson());
    }
    return {{"id", id},
            {"header", header},
            {"content", content},
            {"startLine", startLine},
            {"endLine", endLine},
            {"dialogues", jDialogues},
            {"summary", summary}};
  }
};

class FountainParser {
public:
  static std::vector<Scene> parse(const std::string &text);
};

#endif // FOUNTAIN_PARSER_HPP
