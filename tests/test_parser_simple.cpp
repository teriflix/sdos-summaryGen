#include "../src/FountainParser.cpp" // Including cpp for simple single-file compilation test
#include "../src/FountainParser.hpp"
#include <cassert>
#include <iostream>

int main() {
  std::string testScript = R"(
INT. HOUSE - DAY

John enters the room.

JOHN
Hello world.

EXT. GARDEN - NIGHT

Mary looks at the moon.
)";

  std::vector<Scene> scenes = FountainParser::parse(testScript);

  std::cout << "Parsed " << scenes.size() << " scenes." << std::endl;
  for (const auto &scene : scenes) {
    std::cout << "Scene " << scene.id << ": " << scene.header << std::endl;
    std::cout << "Content:\n" << scene.content << "\n---" << std::endl;
  }

  assert(scenes.size() == 2);
  assert(scenes[0].header == "INT. HOUSE - DAY");
  assert(scenes[1].header == "EXT. GARDEN - NIGHT");

  std::cout << "Test Passed!" << std::endl;
  return 0;
}
