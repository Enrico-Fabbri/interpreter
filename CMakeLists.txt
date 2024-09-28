# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Set the project name
project(IEC VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the directories where CMake should look for header files
include_directories(include)

# Collects all .cpp files
file(GLOB_RECURSE SOURCES "src/*.cpp" "main.cpp")

# Add an executable target with all the sources
add_executable(iec ${SOURCES})
