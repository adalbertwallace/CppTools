# Add the source directory
# target_sources(IncludesAnalyzer PRIVATE src/main.cpp)

# Create the executable
add_executable(IncludesAnalyzer 
    src/main.cpp 
    src/compile_commands_reader.cpp 
    src/path_explorer.cpp
    src/source_file.cpp
)

add_executable(IncludesAnalyzerTest 
    tests/main.cpp 
    src/compile_commands_reader.cpp 
    src/path_explorer.cpp
    src/source_file.cpp
)

target_include_directories(
    IncludesAnalyzerTest 
    PRIVATE 
        include
)

target_include_directories(
    IncludesAnalyzer 
    PRIVATE 
        include
)

target_link_libraries(IncludesAnalyzer
    PRIVATE
        Graph
)
target_link_libraries(IncludesAnalyzerTest
    PRIVATE
        Graph
)