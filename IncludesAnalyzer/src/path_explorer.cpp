#include "path_explorer.hpp"
#include <filesystem>   
#include <vector>
#include <set>

bool PathExplorer::Contains(std::string searchedElement)
{
    std::filesystem::path searchedPath = path;
    searchedPath /= searchedElement;
    // std::cout << "Looking for: " << searchedPath << std::endl;
    if (std::filesystem::exists(searchedPath)) {
        if (std::filesystem::is_regular_file(searchedPath)) {        
            return true;
        }
    }
    return false;
}

void PathExplorer::Walk() {
    std::vector<std::filesystem::path> subdirs = {path}; 

    while (!subdirs.empty()) {
        auto current_path = subdirs.back();
        subdirs.pop_back();
     
    for (const auto& entry : std::filesystem::directory_iterator(current_path)) {
        if (std::filesystem::is_regular_file(entry)) {
            // std::cout << std::filesystem::absolute(entry.path()) << std::endl;
            files.insert(std::filesystem::absolute(entry.path()));
        }
        if (std::filesystem::is_directory(entry)) {
            auto path_to_visit = std::filesystem::absolute(entry.path());
            subdirs.push_back(path_to_visit);
        }   

    }
   } 

}