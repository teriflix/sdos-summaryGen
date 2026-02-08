#include "SessionManager.hpp"
#include "Summarizer.hpp"
#include "Utils.hpp"
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void set_cors_headers(httplib::Response &res) {
  res.set_header("Access-Control-Allow-Origin", "*");
  res.set_header("Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS");
  res.set_header("Access-Control-Allow-Headers", "Content-Type");
}

int main(int argc, char *argv[]) {
  Utils::loadEnv();

  // Default port 8080
  int port = 8080;
  std::string envPort = Utils::getEnv("PORT", "8080");
  try {
    port = std::stoi(envPort);
  } catch (...) {
  }

  httplib::Server svr;

  // Middleware for CORS
  svr.Options(R"(.*)", [](const httplib::Request &, httplib::Response &res) {
    set_cors_headers(res);
    res.status = 200; // Preflight OK
  });

  // POST /session/start
  svr.Post("/session/start", [](const httplib::Request &req,
                                httplib::Response &res) {
    set_cors_headers(res);
    try {
      auto j = json::parse(req.body);
      std::string filepath = j.value("filepath", "");
      int contextWindow = j.value("context_window", 8192);
      std::string modelName = j.value("model_name", "llama3.2");

      if (filepath.empty()) {
        res.status = 400;
        res.set_content(json{{"error", "filepath is required"}}.dump(),
                        "application/json");
        return;
      }

      std::string sessionId = SessionManager::getInstance().createSession(
          filepath, contextWindow, modelName);
      if (sessionId.empty()) {
        res.status = 500;
        res.set_content(json{{"error", "Failed to create session"}}.dump(),
                        "application/json");
        return;
      }

      res.set_content(json{{"session_id", sessionId}}.dump(),
                      "application/json");
    } catch (const std::exception &e) {
      res.status = 400;
      res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
  });

  // POST /summary/scene
  svr.Post("/summary/scene", [](const httplib::Request &req,
                                httplib::Response &res) {
    set_cors_headers(res);
    try {
      auto j = json::parse(req.body);
      std::string sessionId = j.value("session_id", "");
      int sceneId = j.value("scene_id", -1);
      std::string model = j.value("model", "");

      if (sessionId.empty() || sceneId == -1) {
        res.status = 400;
        res.set_content(
            json{{"error", "session_id and scene_id are required"}}.dump(),
            "application/json");
        return;
      }

      auto session = SessionManager::getInstance().getSession(sessionId);
      if (!session) {
        res.status = 404;
        res.set_content(json{{"error", "Session not found"}}.dump(),
                        "application/json");
        return;
      }

      std::string summary = Summarizer::generateSceneSummary(
          *session, sceneId, model, j.value("system_prompt", ""));

      // Check if error message returned (simple heuristic for now)
      if (summary.rfind("Error:", 0) == 0) {
        res.status = 400; // Or 500 depending on error
        res.set_content(json{{"error", summary}}.dump(), "application/json");
        return;
      }

      res.set_content(json{{"summary", summary}}.dump(), "application/json");
    } catch (const std::exception &e) {
      res.status = 400;
      res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
  });

  // POST /summary/story
  svr.Post("/summary/story", [](const httplib::Request &req,
                                httplib::Response &res) {
    set_cors_headers(res);
    try {
      auto j = json::parse(req.body);
      std::string sessionId = j.value("session_id", "");
      std::string model = j.value("model", "");

      if (sessionId.empty()) {
        res.status = 400;
        res.set_content(json{{"error", "session_id is required"}}.dump(),
                        "application/json");
        return;
      }

      auto session = SessionManager::getInstance().getSession(sessionId);
      if (!session) {
        res.status = 404;
        res.set_content(json{{"error", "Session not found"}}.dump(),
                        "application/json");
        return;
      }

      std::string summary = Summarizer::generateStorySummary(
          *session, model, j.value("system_prompt", ""));

      if (summary.rfind("Error:", 0) == 0) {
        res.status = 400;
        res.set_content(json{{"error", summary}}.dump(), "application/json");
        return;
      }

      res.set_content(json{{"summary", summary}}.dump(), "application/json");
    } catch (const std::exception &e) {
      res.status = 400;
      res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
  });

  // POST /summary/character
  svr.Post("/summary/character", [](const httplib::Request &req,
                                    httplib::Response &res) {
    set_cors_headers(res);
    try {
      auto j = json::parse(req.body);
      std::string sessionId = j.value("session_id", "");
      std::string charName = j.value("character_name", "");
      std::string model = j.value("model", "");

      if (sessionId.empty() || charName.empty()) {
        res.status = 400;
        res.set_content(
            json{{"error", "session_id and character_name are required"}}
                .dump(),
            "application/json");
        return;
      }

      auto session = SessionManager::getInstance().getSession(sessionId);
      if (!session) {
        res.status = 404;
        res.set_content(json{{"error", "Session not found"}}.dump(),
                        "application/json");
        return;
      }

      std::string summary = Summarizer::generateCharacterSummary(
          *session, charName, model, j.value("system_prompt", ""));

      if (summary.rfind("Error:", 0) == 0) {
        res.status = 400;
        res.set_content(json{{"error", summary}}.dump(), "application/json");
        return;
      }

      res.set_content(json{{"summary", summary}}.dump(), "application/json");
    } catch (const std::exception &e) {
      res.status = 400;
      res.set_content(json{{"error", e.what()}}.dump(), "application/json");
    }
  });

  // GET /session/{id}
  svr.Get(R"(/session/([^/]+))",
          [](const httplib::Request &req, httplib::Response &res) {
            set_cors_headers(res);
            std::string sessionId = req.matches[1];

            auto session = SessionManager::getInstance().getSession(sessionId);
            if (session) {
              res.set_content(session->toJson().dump(), "application/json");
            } else {
              res.status = 404;
              res.set_content(json{{"error", "Session not found"}}.dump(),
                              "application/json");
            }
          });

  // DELETE /session/{id}
  svr.Delete(R"(/session/([^/]+))", [](const httplib::Request &req,
                                       httplib::Response &res) {
    set_cors_headers(res);
    std::string sessionId = req.matches[1];

    if (SessionManager::getInstance().deleteSession(sessionId)) {
      res.set_content(json{{"status", "deleted"}}.dump(), "application/json");
    } else {
      res.status = 404;
      res.set_content(json{{"error", "Session not found"}}.dump(),
                      "application/json");
    }
  });

  std::cout << "Starting Daemon on port " << port << "..." << std::endl;
  svr.listen("0.0.0.0", port);

  return 0;
}
