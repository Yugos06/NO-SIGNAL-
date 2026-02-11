cmake_minimum_required(VERSION 3.16)
project(no_signal VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(no_signal
    src/main.cpp
    src/game.cpp
)

target_include_directories(no_signal PRIVATE src)
