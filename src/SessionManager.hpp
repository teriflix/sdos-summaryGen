#ifndef SESSION_MANAGER_HPP
#define SESSION_MANAGER_HPP

#include "FountainParser.hpp"
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string>
#include <unordered_map>
#include <vector>

using json = nlohmann::json;

struct CharacterProfile {
  std::string name;
  std::vector<int> sceneIds;
  int dialogueCount;
  std::string summary;

  json toJson() const {
    return {{"name", name},
            {"sceneIds", sceneIds},
            {"dialogueCount", dialogueCount},
            {"summary", summary}};
  }
};

struct SessionConfig {
  int contextWindow;
  std::string modelName;

  json toJson() const {
    return {{"contextWindow", contextWindow}, {"modelName", modelName}};
  }

  static SessionConfig fromJson(const json &j) {
    SessionConfig c;
    c.contextWindow = j.value("contextWindow", 8192);
    c.modelName = j.value("modelName", "");
    return c;
  }
};

struct Session {
  std::string id;
  std::string filepath;
  std::vector<Scene> scenes;
  std::unordered_map<std::string, CharacterProfile> characters;
  SessionConfig config;

  // Cache for summaries (could be more complex, but simple map for now)
  // Map SceneID -> Summary is already in Scene struct, but maybe we want
  // separate cache? Let's rely on Scene struct for scene summaries. Story
  // summary:
  std::string storySummary;

  // Metadata
  long long createdAt;
  long long lastModified;

  json toJson() const;
  static Session fromJson(const json &j);
};

class SessionManager {
public:
  static SessionManager &getInstance();

  std::string createSession(const std::string &filepath, int contextWindow,
                            const std::string &modelName);
  std::shared_ptr<Session> getSession(const std::string &sessionId);
  bool deleteSession(const std::string &sessionId);
  void saveSession(const std::string &sessionId);

  // Helper to build character map from scenes
  void buildCharacterMap(Session &session);

private:
  SessionManager();
  ~SessionManager() = default;

  std::unordered_map<std::string, std::shared_ptr<Session>> sessions;
  std::mutex sessionMutex;
  std::string storageDir = "sessions";

  std::string generateUUID();
  void loadAllSessions();
};

#endif // SESSION_MANAGER_HPP
