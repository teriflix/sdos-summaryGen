#ifndef FOUNTAIN_PARSER_HPP
#define FOUNTAIN_PARSER_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

struct Scene {
  int id;
  std::string header;
  std::string content;
  std::string summary; // Will be filled later

  json toJson() const {
    return {{"id", id},
            {"header", header},
            {"content", content},
            {"summary", summary}};
  }
};

class FountainParser {
public:
  static std::vector<Scene> parse(const std::string &text);
};

#endif // FOUNTAIN_PARSER_HPP
