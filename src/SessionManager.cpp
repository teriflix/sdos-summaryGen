#include "SessionManager.hpp"
#include "Utils.hpp"
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <sys/stat.h>

// Assuming C++17
namespace fs = std::filesystem;

// Simple UUID generator
std::string SessionManager::generateUUID() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> dis(0, 15);
  std::uniform_int_distribution<> dis2(8, 11);

  std::stringstream ss;
  ss << std::hex;
  for (int i = 0; i < 8; i++)
    ss << dis(gen);
  ss << "-";
  for (int i = 0; i < 4; i++)
    ss << dis(gen);
  ss << "-4"; // UUID version 4
  for (int i = 0; i < 3; i++)
    ss << dis(gen);
  ss << "-";
  ss << dis2(gen); // Variant
  for (int i = 0; i < 3; i++)
    ss << dis(gen);
  ss << "-";
  for (int i = 0; i < 12; i++)
    ss << dis(gen);
  return ss.str();
}

SessionManager &SessionManager::getInstance() {
  static SessionManager instance;
  return instance;
}

SessionManager::SessionManager() {
  // Create sessions directory if not exists
  if (!fs::exists(storageDir)) {
    fs::create_directory(storageDir);
  }
  loadAllSessions();
}

// Internal helper logic for save
void SaveSessionToFile(const Session &session, const std::string &dir) {
  std::string path = dir + "/" + session.id + ".json";
  std::ofstream out(path);
  if (out.is_open()) {
    out << session.toJson().dump(4);
  }
}

std::string SessionManager::createSession(const std::string &filepath,
                                          int contextWindow,
                                          const std::string &modelName) {
  std::lock_guard<std::mutex> lock(sessionMutex);

  auto session = std::make_shared<Session>();
  session->id = generateUUID();
  session->filepath = filepath;
  session->config.contextWindow = contextWindow;
  session->config.modelName = modelName;
  session->createdAt = std::time(nullptr);
  session->lastModified = std::time(nullptr);

  // Parse file
  try {
    std::string content = Utils::readFile(filepath);
    session->scenes = FountainParser::parse(content);
    buildCharacterMap(*session);
  } catch (const std::exception &e) {
    std::cerr << "Error parsing file: " << e.what() << std::endl;
    return "";
  }

  sessions[session->id] = session;
  SaveSessionToFile(*session, storageDir);
  return session->id;
}

std::shared_ptr<Session>
SessionManager::getSession(const std::string &sessionId) {
  std::lock_guard<std::mutex> lock(sessionMutex);
  auto it = sessions.find(sessionId);
  if (it != sessions.end()) {
    return it->second;
  }
  return nullptr;
}

bool SessionManager::deleteSession(const std::string &sessionId) {
  std::lock_guard<std::mutex> lock(sessionMutex);
  auto it = sessions.find(sessionId);
  if (it != sessions.end()) {
    sessions.erase(it);
    std::string path = storageDir + "/" + sessionId + ".json";
    fs::remove(path);
    return true;
  }
  return false;
}

void SessionManager::saveSession(const std::string &sessionId) {
  std::lock_guard<std::mutex> lock(sessionMutex);
  auto it = sessions.find(sessionId);
  if (it != sessions.end()) {
    SaveSessionToFile(*(it->second), storageDir);
  }
}

void SessionManager::buildCharacterMap(Session &session) {
  session.characters.clear();
  for (const auto &scene : session.scenes) {
    for (const auto &diag : scene.dialogues) {
      std::string name = diag.character;
      if (session.characters.find(name) == session.characters.end()) {
        CharacterProfile p;
        p.name = name;
        p.dialogueCount = 0;
        p.summary = "";
        session.characters[name] = p;
      }
      session.characters[name].sceneIds.push_back(scene.id);
      session.characters[name].dialogueCount++;
    }
  }

  // Remove duplicate scene IDs for characters
  for (auto &pair : session.characters) {
    auto &ids = pair.second.sceneIds;
    std::sort(ids.begin(), ids.end());
    ids.erase(std::unique(ids.begin(), ids.end()), ids.end());
  }
}

void SessionManager::loadAllSessions() {
  if (!fs::exists(storageDir))
    return;

  for (const auto &entry : fs::directory_iterator(storageDir)) {
    if (entry.path().extension() == ".json") {
      try {
        std::ifstream i(entry.path());
        json j;
        i >> j;
        Session s = Session::fromJson(j);

        // We don't need to lock here because this is called from constructor
        sessions[s.id] = std::make_shared<Session>(s);
      } catch (const std::exception &e) {
        std::cerr << "Failed to load session " << entry.path() << ": "
                  << e.what() << std::endl;
      }
    }
  }
}

// Session JSON implementation (Need to implement here since it's declared in
// header)
json Session::toJson() const {
  json jScenes = json::array();
  for (const auto &s : scenes)
    jScenes.push_back(s.toJson());

  json jChars = json::object();
  for (const auto &pair : characters)
    jChars[pair.first] = pair.second.toJson();

  return {{"id", id},
          {"filepath", filepath},
          {"scenes", jScenes},
          {"characters", jChars},
          {"config", config.toJson()},
          {"storySummary", storySummary},
          {"createdAt", createdAt},
          {"lastModified", lastModified}};
}

Session Session::fromJson(const json &j) {
  Session s;
  s.id = j.value("id", "");
  s.filepath = j.value("filepath", "");

  if (j.contains("scenes")) {
    for (const auto &item : j["scenes"]) {
      Scene scene;
      scene.id = item.value("id", 0);
      scene.header = item.value("header", "");
      scene.content = item.value("content", "");
      scene.summary = item.value("summary", "");
      scene.startLine = item.value("startLine", 0);
      scene.endLine = item.value("endLine", 0);

      if (item.contains("dialogues")) {
        for (const auto &dItem : item["dialogues"]) {
          Dialogue d;
          d.character = dItem.value("character", "");
          d.text = dItem.value("text", "");
          d.lineNumber = dItem.value("lineNumber", 0);
          scene.dialogues.push_back(d);
        }
      }
      s.scenes.push_back(scene);
    }
  }

  // Characters
  if (j.contains("characters")) {
    for (auto &el : j["characters"].items()) {
      CharacterProfile p;
      p.name = el.value().value("name", "");
      p.dialogueCount = el.value().value("dialogueCount", 0);
      p.summary = el.value().value("summary", "");
      if (el.value().contains("sceneIds")) {
        for (auto &id : el.value()["sceneIds"]) {
          p.sceneIds.push_back(id);
        }
      }
      s.characters[el.key()] = p;
    }
  }

  if (j.contains("config")) {
    s.config = SessionConfig::fromJson(j["config"]);
  }

  s.storySummary = j.value("storySummary", "");
  s.createdAt = j.value("createdAt", 0);
  s.lastModified = j.value("lastModified", 0);

  return s;
}
