cmake_minimum_required(VERSION 3.15)
project(BitmapViewer VERSION 1.0.0 LANGUAGES CXX)

# Enforce strict C++98 standard
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define the source files and executable target
add_executable(BitmapViewer src/main.cpp)

# Link Windows GDI and Common Dialog libraries
target_link_libraries(BitmapViewer PRIVATE gdi32 comdlg32)

# Enable compiler warnings for safety
if(MSVC)
    target_compile_options(BitmapViewer PRIVATE /W4)
else()
    target_compile_options(BitmapViewer PRIVATE -Wall -Wextra -pedantic)
endif()
