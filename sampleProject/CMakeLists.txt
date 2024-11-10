# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(CppToolsTestProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(CppToolsTestProject
    src/main.cpp
) 

target_include_directories(
    CppToolsTestProject 
    PRIVATE 
        include
)
# Add subdirectories for different components of the project

# Export compile commands
