#pragma once
#include <string>
#include <vector>
#include "includee.hpp"

class SourceFileReader{
public:
    SourceFileReader(const std::string& source_file_path):
    sourceFilePath(source_file_path){
        ;
    }
    
    std::vector<Includee> ReadIncludes() const;
    private:
    std::string sourceFilePath;
};