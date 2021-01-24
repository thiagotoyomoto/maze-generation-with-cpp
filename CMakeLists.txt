cmake_minimum_required(VERSION 3.0.0)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

set(USE_PTHREAD FALSE)

if(USE_PTHREAD)
    set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
    set(THREADS_PREFER_PTHREAD_FLAG TRUE)
endif()

project(maze-generator-with-cpp VERSION 0.1.0)

if(USE_PTHREAD)
    find_package(Threads REQUIRED)
endif()
find_package(SFML COMPONENTS graphics window system)

add_executable(${PROJECT_NAME} src/main.cpp src/Maze.cpp)

if(USE_PTHREAD)
    target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
endif()
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio)
