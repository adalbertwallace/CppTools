#pragma once
#include <iostream>

class PathExplorer {
public:
    PathExplorer(const std::string& path): path(path) {
        ;
    }
    void Explore();
    
    bool Contains(std::string searchedElement);
    
    private:
    std::string path;
};