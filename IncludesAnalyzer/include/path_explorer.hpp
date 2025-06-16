#pragma once
#include <iostream>
#include <filesystem>   
#include <set>
class PathExplorer {
public:
    PathExplorer(const std::string& path): path(path) {
        ;
    }

    bool Contains(std::string searchedElement);
    
    private:
    void Walk();
    std::set<std::filesystem::path> files;    
    std::string path;
};