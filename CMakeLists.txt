# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(CppTools)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories for different components of the project
add_subdirectory(IncludesAnalyzer)
# Export compile commands
