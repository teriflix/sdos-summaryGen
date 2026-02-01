#include "OllamaClient.hpp"
#include "Utils.hpp"
#include <curl/curl.h>
#include <iostream>

size_t OllamaClient::WriteCallback(void *contents, size_t size, size_t nmemb,
                                   void *userp) {
  ((std::string *)userp)->append((char *)contents, size * nmemb);
  return size * nmemb;
}

std::string OllamaClient::chatCompletion(const std::string &systemPrompt,
                                         const std::vector<Message> &messages,
                                         const std::string &modelOverride) {
  CURL *curl;
  CURLcode res;
  std::string readBuffer;

  std::string model = modelOverride;
  if (model.empty()) {
    model = Utils::getEnv("OLLAMA_MODEL", "llama3.1:8b-instruct-q4_0");
  }
  std::string baseUrl =
      Utils::getEnv("OLLAMA_BASE_URL", "http://127.0.0.1:11434");
  std::string fullUrl = baseUrl + "/api/chat";

  curl = curl_easy_init();
  if (curl) {
    json payload;
    payload["model"] = model;
    payload["stream"] = false;

    json msgsJson = json::array();
    if (!systemPrompt.empty()) {
      msgsJson.push_back({{"role", "system"}, {"content", systemPrompt}});
    }

    for (const auto &msg : messages) {
      msgsJson.push_back(msg.toJson());
    }
    payload["messages"] = msgsJson;

    std::string payloadStr = payload.dump();

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, fullUrl.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    res = curl_easy_perform(curl);

    if (res != CURLE_OK) {
      std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res)
                << std::endl;
    }

    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
  }

  try {
    auto responseJson = json::parse(readBuffer);
    if (responseJson.contains("message") &&
        responseJson["message"].contains("content")) {
      return responseJson["message"]["content"];
    } else {
      std::cerr << "Error: Unexpected response format from Ollama: "
                << readBuffer << std::endl;
      return "Error: Could not retrieve summary from Ollama.";
    }
  } catch (const std::exception &e) {
    std::cerr << "JSON Parse Error: " << e.what()
              << "\nResponse: " << readBuffer << std::endl;
    return "Error: JSON Parsing failed.";
  }
}
