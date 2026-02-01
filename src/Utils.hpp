#ifndef UTILS_HPP
#define UTILS_HPP

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class Utils {
public:
  static std::string readFile(const std::string &path) {
    std::ifstream file(path);
    if (!file.is_open()) {
      throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
  }

  static std::string trim(const std::string &str) {
    auto start = str.begin();
    while (start != str.end() && std::isspace(*start)) {
      start++;
    }

    auto end = str.end();
    do {
      end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));

    return std::string(start, end + 1);
  }

  // Simple .env loader
  static void loadEnv(const std::string &path = ".env") {
    std::ifstream file(path);
    if (!file.is_open()) {
      // It's okay if .env doesn't exist, we'll use defaults or system env vars
      return;
    }

    std::string line;
    while (std::getline(file, line)) {
      std::string trimmed = trim(line);
      if (trimmed.empty() || trimmed[0] == '#')
        continue;

      auto delimiterPos = trimmed.find('=');
      if (delimiterPos != std::string::npos) {
        std::string key = trim(trimmed.substr(0, delimiterPos));
        std::string value = trim(trimmed.substr(delimiterPos + 1));

        // Remove quotes if present
        if (value.size() >= 2 && value.front() == '"' && value.back() == '"') {
          value = value.substr(1, value.size() - 2);
        }

        // Set env var if not already set
        setenv(key.c_str(), value.c_str(),
               0); // 0 means do not overwrite if exists
      }
    }
  }

  static std::string getEnv(const std::string &key,
                            const std::string &defaultValue) {
    const char *val = std::getenv(key.c_str());
    return val ? std::string(val) : defaultValue;
  }
};

#endif // UTILS_HPP
