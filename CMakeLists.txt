cmake_minimum_required(VERSION 3.20)
project(C8E VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -fexceptions")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "${CMAKE_CXX_FLAGS} -pg")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -Wextra -O3")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

#include(FetchContent)
#fetchcontent_declare(
#  googletest
#  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
#)
find_package(SDL2 REQUIRED)

add_executable(${PROJECT_NAME} main.cc src/chip8.cc)
target_include_directories(${PROJECT_NAME} PRIVATE includes)
target_link_libraries(${PROJECT_NAME} SDL2::SDL2main SDL2::SDL2)
