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

    void ApplyMapping(const std::string& from, const std::string& to) {
        directory.replace(directory.find(from), from.length(), to);

        sourceFile.replace(sourceFile.find(from), from.length(), to);
        
        for (auto& i : includePaths) {
            if (i.find(from) != std::string::npos) {
                i.replace(i.find(from), from.length(), to);
            }
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

    std::string ToString() const {
        std::string result = "Directory: " + directory + "\n";
        result += "Source file: " + sourceFile + "\n";
        result += "Include paths:\n";
        for (auto i : includePaths) {
            result += i + "\n";
        }
        return result;
    }
private:
    std::string directory;
    std::string sourceFile;
    std::vector<std::string> includePaths;
};

