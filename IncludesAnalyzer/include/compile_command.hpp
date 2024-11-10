#pragma once

#include <string>

class CompileCommand{

public:
    CompileCommand(const std::string& directory, const std::string& sourceFile, const std::vector<std::string>& includePaths):
    directory(directory), sourceFile(sourceFile)
    {
        if (directory.back() != '/') {
            this->directory += '/';
        } 
        for (auto i : includePaths) {
            this->includePaths.emplace_back(this->directory + i);
        }
    }

    std::string GetDirectory() const {
        return directory;
    }
    std::string GetSourceFile() const {
        return sourceFile;
    }
    std::vector<std::string> GetIncludePaths() const {
        return includePaths;
    }
private:
    std::string directory;
    std::string sourceFile;
    std::vector<std::string> includePaths;
};

