cmake_minimum_required(VERSION 3.15)
project(LayingGrass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(includes)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(LayingGrassClient client.cpp ${SOURCES})
add_executable(LayingGrassServer server.cpp ${SOURCES})

