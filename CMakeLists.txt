cmake_minimum_required(VERSION 3.15)
project(LayingGrass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

set(SFML_CMAKE_ARGS
    -DSFML_BUILD_DOC=OFF
    -DSFML_BUILD_EXAMPLES=OFF
)

if(UNIX AND NOT APPLE)
    find_path(UDEV_INCLUDE_DIR libudev.h PATHS /usr/include /usr/local/include)
    find_library(UDEV_LIBRARY udev PATHS /usr/lib /usr/local/lib /lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu)
    if(UDEV_INCLUDE_DIR AND UDEV_LIBRARY)
        list(APPEND SFML_CMAKE_ARGS
            -DUDEV_PATH_INCLUDES=${UDEV_INCLUDE_DIR}
            -DUDEV_PATH_LIB=${UDEV_LIBRARY}
        )
    endif()
endif()

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
  CMAKE_ARGS ${SFML_CMAKE_ARGS}
)
FetchContent_MakeAvailable(SFML)

include_directories(includes)

file(GLOB_RECURSE SOURCES "src/*.cpp")

add_executable(LayingGrassClient client.cpp ${SOURCES})
target_link_libraries(LayingGrassClient sfml-graphics sfml-window sfml-system)

add_executable(LayingGrassServer server.cpp ${SOURCES})
target_link_libraries(LayingGrassServer sfml-system)

