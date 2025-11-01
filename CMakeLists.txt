cmake_minimum_required(VERSION 3.15)
project(LayingGrass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

include_directories(includes)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(LayingGrassClient client.cpp ${SOURCES})
target_link_libraries(LayingGrassClient sfml-graphics sfml-window sfml-system)

add_executable(LayingGrassServer server.cpp ${SOURCES})
target_link_libraries(LayingGrassServer sfml-system)

