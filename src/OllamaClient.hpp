#ifndef OLLAMA_CLIENT_HPP
#define OLLAMA_CLIENT_HPP

#include <nlohmann/json.hpp>
#include <string>
#include <vector>

using json = nlohmann::json;

struct Message {
  std::string role;
  std::string content;

  json toJson() const { return {{"role", role}, {"content", content}}; }
};

class OllamaClient {
public:
  static std::string chatCompletion(const std::string &systemPrompt,
                                    const std::vector<Message> &messages,
                                    const std::string &model = "");

private:
  static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                              void *userp);
};

#endif // OLLAMA_CLIENT_HPP
