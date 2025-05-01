# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

# Project name
project(CppTools)

# Set the output directory for executables
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Set the output directory for shared and static libraries
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Set the output directory for static libraries (archives)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Add subdirectories for different components of the project
add_subdirectory(IncludesAnalyzer)
add_subdirectory(Graph)
# Export compile commands
