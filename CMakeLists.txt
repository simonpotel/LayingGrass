cmake_minimum_required(VERSION 3.15)
project(LayingGrass)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(FetchContent)

set(SFML_BUILD_DOC OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

if(UNIX AND NOT APPLE)
    find_path(UDEV_INCLUDE_DIR libudev.h PATHS /usr/include /usr/local/include)
    find_library(UDEV_LIBRARY udev PATHS /usr/lib /usr/local/lib /lib/x86_64-linux-gnu /usr/lib/x86_64-linux-gnu)
    if(UDEV_INCLUDE_DIR AND UDEV_LIBRARY)
        set(UDEV_PATH_INCLUDES ${UDEV_INCLUDE_DIR} CACHE PATH "" FORCE)
        set(UDEV_PATH_LIB ${UDEV_LIBRARY} CACHE FILEPATH "" FORCE)
    endif()
endif()

FetchContent_Declare(
  SFML
  GIT_REPOSITORY https://github.com/SFML/SFML.git
  GIT_TAG 2.6.1
)
FetchContent_MakeAvailable(SFML)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include_directories(includes)

file(GLOB_RECURSE SOURCES "src/*.cpp")

function(copy_sfml_dlls target)
    if(WIN32)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:sfml-graphics>
                $<TARGET_FILE:sfml-window>
                $<TARGET_FILE:sfml-system>
                $<TARGET_FILE_DIR:${target}>
        )
    endif()
endfunction()

function(copy_gtest_dlls target)
    if(WIN32)
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:gtest>
                $<TARGET_FILE:gtest_main>
                $<TARGET_FILE_DIR:${target}>
        )
    endif()
endfunction()

add_executable(LayingGrassClient client.cpp ${SOURCES})
target_link_libraries(LayingGrassClient PRIVATE sfml-graphics sfml-window sfml-system)
copy_sfml_dlls(LayingGrassClient)
if(UNIX)
    target_link_libraries(LayingGrassClient PRIVATE pthread)
endif()

if(APPLE)
    set_target_properties(LayingGrassClient PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()

add_executable(LayingGrassServer server.cpp ${SOURCES})
target_link_libraries(LayingGrassServer PRIVATE sfml-window sfml-system)
copy_sfml_dlls(LayingGrassServer)
if(UNIX)
    target_link_libraries(LayingGrassServer PRIVATE pthread)
endif()

if(APPLE)
    set_target_properties(LayingGrassServer PROPERTIES
        MACOSX_BUNDLE FALSE
    )
endif()

enable_testing()
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

if(TEST_SOURCES)
    add_executable(LayingGrassTests ${TEST_SOURCES} ${SOURCES})
    target_link_libraries(LayingGrassTests PRIVATE
        GTest::gtest 
        GTest::gtest_main 
        sfml-window
        sfml-system
    )
    
    copy_sfml_dlls(LayingGrassTests)
    copy_gtest_dlls(LayingGrassTests)
    
    include(GoogleTest)
    gtest_discover_tests(LayingGrassTests)
endif()