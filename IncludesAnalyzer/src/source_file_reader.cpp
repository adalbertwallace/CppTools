#include "source_file_reader.hpp"
#include "includee.hpp"
#include <regex>
#include <fstream>      // std::ifstream
#include <iostream>     // std::cout

std::vector<Includee> SourceFileReader::ReadIncludes() const {
        std::cout << "Reading file: "<<sourceFilePath<<std::endl;
        std::ifstream sourceFile(sourceFilePath.c_str());
        std::string line;
        std::regex includeRegex("^\\s*#include\\s*<(.*)>");
        std::regex includeRegex2("^\\s*#include\\s*\"(.*)\"");
        std::vector<Includee> includes;
        while (std::getline(sourceFile, line)) {
            std::smatch match;
            if (std::regex_search(line, match, includeRegex)) {
                includes.push_back(Includee(match[1], IncludeeType::InBrackets));
            } else if (std::regex_search(line, match, includeRegex2)) {
                includes.push_back(Includee(match[1], IncludeeType::InQuotes));
            }
        }
        return includes;
    }