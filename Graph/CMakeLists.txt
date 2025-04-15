cmake_minimum_required(VERSION 3.12)

project(Graph)

set(CMAKE_CXX_STANDARD 20)

add_library(Graph
    src/graph.cpp
)

target_include_directories(
    Graph
    PUBLIC 
        include
)

enable_testing()

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(test_graph tests/test_graph.cpp)

target_include_directories(
    test_graph
    PUBLIC 
        include
)


# target_link_libraries(test_graph PRIVATE Graph ${GTEST_LIBRARIES} pthread)
target_link_libraries(test_graph PRIVATE Graph gtest_main gtest pthread)
add_test(NAME test_graph COMMAND test_graph)