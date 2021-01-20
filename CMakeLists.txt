cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_GENERATOR "Ninja")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")

set(CMAKE_CXX_STANDARD 17)

project(maze-generation VERSION 0.1.0)

find_package(SFML COMPONENTS graphics window system)

add_executable(maze-generation src/main.cpp)

target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)
