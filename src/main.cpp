#include "FountainParser.hpp"
#include "Summarizer.hpp"
#include "Utils.hpp"
#include <algorithm>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <regex>
#include <string>
#include <sys/stat.h>
#include <vector>

// Platform-specific mkdir
#ifdef _WIN32
#include <direct.h>
#define MAKE_DIR(dir) _mkdir(dir)
#else
#include <sys/stat.h>
#define MAKE_DIR(dir) mkdir(dir, 0777)
#endif

void printUsage(const char *progName) {
  std::cout << "Usage: " << progName << " <filename> [options]\n";
  std::cout << "Options:\n";
  std::cout << "  --scene <scene_header_fragment>   Generate summary for a "
               "specific scene.\n";
  std::cout << "  --character <character_name>      Generate profile for a "
               "specific character.\n";
  std::cout << "  --help                            Show this help message.\n";
  std::cout << "\nIf no options are provided, a full story summary AND top 5 "
               "character profiles are generated.\n";
}

void saveLog(const std::string &originalFilename, const std::string &type,
             const std::string &content) {
  struct stat info;
  if (stat("log", &info) != 0) {
    MAKE_DIR("log");
  }

  auto t = std::time(nullptr);
  auto tm = *std::localtime(&t);
  std::ostringstream oss;
  oss << std::put_time(&tm, "%Y%m%d_%H%M%S");
  std::string timestamp = oss.str();

  std::string baseName = originalFilename;
  size_t lastSlash = baseName.find_last_of("/\\");
  if (lastSlash != std::string::npos)
    baseName = baseName.substr(lastSlash + 1);
  size_t lastDot = baseName.find_last_of(".");
  if (lastDot != std::string::npos)
    baseName = baseName.substr(0, lastDot);

  std::string safeType = type;
  std::replace_if(
      safeType.begin(), safeType.end(), [](char c) { return !isalnum(c); },
      '_');

  std::string logFilename =
      "log/" + baseName + "_" + timestamp + "_" + safeType + ".txt";

  std::ofstream logFile(logFilename);
  if (logFile.is_open()) {
    logFile << content;
    std::cout << "Log saved to: " << logFilename << "\n";
  } else {
    std::cerr << "Failed to save log file: " << logFilename << "\n";
  }
}

// Helper to extract dialogue samples for a specific character
std::string extractDialogueSamples(const std::string &content,
                                   const std::string &character,
                                   int limit = 50) {
  std::stringstream ss(content);
  std::string line;
  std::vector<std::string> dialogues;
  bool nextIsDialogue = false;

  while (std::getline(ss, line)) {
    std::string trimmed = Utils::trim(line);
    if (trimmed.empty())
      continue;

    if (nextIsDialogue) {
      // Check if it's potentially another character name (all caps)
      // Heuristic: All caps, or contains parenthetical, and shortish.
      bool isNextChar = false;
      if (std::all_of(trimmed.begin(), trimmed.end(),
                      [](unsigned char c) {
                        return std::isupper(c) || std::ispunct(c) ||
                               std::isspace(c);
                      }) &&
          trimmed.length() < 50) {
        isNextChar = true;
      }

      if (trimmed == character) { // Same character again?
        nextIsDialogue = true;
      } else if (isNextChar) {
        // It's another character
        nextIsDialogue = false;
      } else {
        // It's dialogue
        dialogues.push_back(trimmed);
        if (dialogues.size() > limit)
          break;
      }
    }

    // Check if this line is the character
    if (trimmed.find(character) != std::string::npos && trimmed.length() < 50) {
      // Basic check: start matches
      if (trimmed.substr(0, character.length()) == character) {
        nextIsDialogue = true;
      }
    }
  }

  std::string samples;
  for (const auto &d : dialogues)
    samples += d + "\n";
  return samples;
}

// Helper to identify top characters by dialogue frequency
std::vector<std::string> getTopCharacters(const std::string &content,
                                          int topN = 5) {
  std::map<std::string, int> counts;
  std::stringstream ss(content);
  std::string line;

  // Regex for exact character name lines (uppercase, maybe parentheticals)
  // Excluding known transitions/headers like INT. EXT. CUT TO:
  std::regex charLineRegex(R"(^[A-Z][A-Z0-9\s\.\(\)\'-]+$)");

  while (std::getline(ss, line)) {
    std::string trimmed = Utils::trim(line);
    if (trimmed.empty())
      continue;

    if (std::regex_match(trimmed, charLineRegex)) {
      // Filter out common scene headers or transitions if they slip through
      if (trimmed.find("INT.") == 0 || trimmed.find("EXT.") == 0 ||
          trimmed.find("CUT TO:") != std::string::npos) {
        continue;
      }

      // Clean name (remove parentheticals for counting? e.g. JOHN (V.O.) ->
      // JOHN)
      std::string name = trimmed;
      size_t paren = name.find('(');
      if (paren != std::string::npos) {
        name = Utils::trim(name.substr(0, paren));
      }

      if (!name.empty() && name.length() > 1) { // Ignore single letters usually
        counts[name]++;
      }
    }
  }

  // Sort by count
  std::vector<std::pair<std::string, int>> sortedChars(counts.begin(),
                                                       counts.end());
  std::sort(sortedChars.begin(), sortedChars.end(),
            [](const auto &a, const auto &b) { return a.second > b.second; });

  std::vector<std::string> topChars;
  for (int i = 0; i < std::min((int)sortedChars.size(), topN); ++i) {
    topChars.push_back(sortedChars[i].first);
  }
  return topChars;
}

int main(int argc, char *argv[]) {
  Utils::loadEnv();

  if (argc < 2) {
    printUsage(argv[0]);
    return 1;
  }

  std::string filename;
  std::string mode = "story";
  std::string target;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--help") {
      printUsage(argv[0]);
      return 0;
    } else if (arg == "--scene") {
      if (i + 1 < argc) {
        mode = "scene";
        target = argv[++i];
      } else {
        std::cerr << "Error: --scene requires an argument.\n";
        return 1;
      }
    } else if (arg == "--character") {
      if (i + 1 < argc) {
        mode = "character";
        target = argv[++i];
      } else {
        std::cerr << "Error: --character requires an argument.\n";
        return 1;
      }
    } else {
      if (filename.empty()) {
        filename = arg;
      } else {
        std::cerr << "Error: Unexpected argument '" << arg << "'.\n";
        return 1;
      }
    }
  }

  if (filename.empty()) {
    std::cerr << "Error: No filename provided.\n";
    printUsage(argv[0]);
    return 1;
  }

  try {
    std::string content = Utils::readFile(filename);
    std::vector<Scene> scenes = FountainParser::parse(content);

    if (mode == "scene") {
      bool found = false;
      std::vector<Scene> targetScenes;
      for (auto &s : scenes) {
        if (s.header.find(target) != std::string::npos ||
            (s.id == 0 && target == "START")) {
          targetScenes.push_back(s);
          found = true;
        }
      }

      if (!found) {
        std::cerr << "Error: No scenes found matching '" << target << "'.\n";
        return 1;
      }

      Summarizer::generateSceneSummaries(targetScenes);

      std::stringstream logContent;
      logContent << "SCENE SUMMARY REPORT\n";
      logContent << "Target: " << target << "\n\n";

      for (const auto &s : targetScenes) {
        std::string header = "\n=== SCENE: " + s.header + " ===\n";
        std::string body = s.summary + "\n";
        std::cout << header << body;
        logContent << header << body;
      }
      saveLog(filename, "scene_" + target, logContent.str());

    } else if (mode == "character") {
      std::string samples = extractDialogueSamples(content, target);

      if (samples.empty()) {
        std::cout << "Warning: No dialogue found for " << target
                  << ". Profile may be generic.\n";
      }

      std::cout << "\n=== CHARACTER PROFILE: " << target << " ===\n";
      std::string profile =
          Summarizer::generateCharacterProfile(target, samples);
      std::cout << profile << "\n";

      saveLog(filename, "character_" + target,
              "CHARACTER PROFILE: " + target + "\n\n" + profile);

    } else {
      // Story Summary (Default Mode)
      if (scenes.empty()) {
        std::cout << "No scenes detected for story summary.\n";
      } else {
        // 1. Scene Summaries
        std::cout << "Generating scene summaries for " << scenes.size()
                  << " scenes...\n";
        Summarizer::generateSceneSummaries(scenes);

        // 2. Story Summary
        std::cout << "Generating full story summary...\n";
        std::string storySummary = Summarizer::generateStorySummary(scenes);
        std::cout << "\n=== STORY SUMMARY ===\n";
        std::cout << storySummary << "\n";

        std::stringstream logContent;
        logContent << "STORY SUMMARY REPORT\n\n=== SYNOPSIS ===\n"
                   << storySummary << "\n\n";

        // 3. Top 5 Characters
        std::cout << "\nGenerating profiles for top 5 characters...\n";
        std::vector<std::string> topChars = getTopCharacters(content, 5);

        logContent << "=== CHARACTER PROFILES ===\n";
        for (const auto &charName : topChars) {
          std::cout << "Processing " << charName << "...\n";
          std::string samples = extractDialogueSamples(content, charName);
          std::string profile =
              Summarizer::generateCharacterProfile(charName, samples);

          std::cout << "\n--- " << charName << " ---\n" << profile << "\n";
          logContent << "\n--- " << charName << " ---\n" << profile << "\n";
        }

        // 4. Scene Breakdown
        logContent << "\n=== SCENE BREAKDOWN ===\n";
        for (const auto &s : scenes) {
          logContent << "SCENE " << s.id << ": " << s.header << "\n"
                     << s.summary << "\n\n";
        }

        saveLog(filename, "story_full", logContent.str());
      }
    }

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << "\n";
    return 1;
  }

  return 0;
}
