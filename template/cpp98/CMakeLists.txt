cmake_minimum_required(VERSION 3.15)
project(Cpp98Project VERSION 1.0.0 LANGUAGES CXX)

# Enforce strict C++98 standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the source files and executable target
add_executable(Cpp98Project src/main.cpp)

# Enable compiler warnings for safety
if(MSVC)
    target_compile_options(Cpp98Project PRIVATE /W4)
else()
    target_compile_options(Cpp98Project PRIVATE -Wall -Wextra -pedantic)
endif()
