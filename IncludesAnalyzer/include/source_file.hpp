#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <regex>
#include <memory>
#include <map>
#include <set>
#include "edge.hpp"
#include "graph.hpp"
#include "node.hpp"
#include "compile_command.hpp"
#include "common.hpp"

std::vector<std::string> ReadIncludes(std::string path);

class FilesGraph {
private:
    Graph graph;
    std::set<std::string> processedNodes;
    std::vector<std::string> source_files;
public:
    FilesGraph();
    ~FilesGraph();
    
    std::vector<std::string> GetSources() const{
        return source_files;
    }
    // void ForEachSourceFile(std::function<void(std::string)> f) {
        
    // }

    void UpdateNetwork(CompileCommand &cc);
    void PrintStats();
    std::vector<std::string> GetDependenciesOf(std::string include);
    void GetUsersOf(std::string include);
    void PrintU();
    void Print();
};