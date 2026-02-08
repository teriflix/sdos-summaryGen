cmake_minimum_required(VERSION 3.10)
project(FountainTooCLI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find libcurl
find_package(CURL REQUIRED)
find_package(Threads REQUIRED)

# Include directories
include_directories(include)
include_directories(${CURL_INCLUDE_DIRS})

# Source files
set(SOURCES
    src/main.cpp
    src/FountainParser.cpp
    src/OllamaClient.cpp
    src/Summarizer.cpp
    src/SessionManager.cpp
)

# Executable
add_executable(fountain_tool ${SOURCES})

# Link libraries
target_link_libraries(fountain_tool ${CURL_LIBRARIES} Threads::Threads)
