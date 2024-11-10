#include "path_explorer.hpp"
#include <filesystem>   

void PathExplorer::Explore() {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry)) {
            std::cout << entry.path().filename() << std::endl;
        }
    }
   
    std::cout << "Exploring path: " << path << std::endl;
}

bool PathExplorer::Contains(std::string searchedElement)
{
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (std::filesystem::is_regular_file(entry)) {
            if (entry.path().filename() == searchedElement) {
                return true;
            }
        }
    }
    return false;
}