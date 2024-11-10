#include "compile_commands_reader.hpp"
#include <regex>
#include <string>
#include <fstream>
    
    std::vector<std::string> f1(std::string line) {
        std::vector<std::string> includePaths;

        // Regular expression to capture the include directories starting with -I
        std::regex include_regex(R"( -I(\S+))");

        // Iterator to find all matches of the regex in the input string
        auto begin = std::sregex_iterator(line.begin(), line.end(), include_regex);
        auto end = std::sregex_iterator();

        // Loop through the matches and print the captured directories
        for (std::sregex_iterator i = begin; i != end; ++i) {
            std::smatch match = *i;
            std::string include_dir = match.str(1); // Capture group 1, the directory
            includePaths.emplace_back(include_dir);
        }
        return includePaths;
    }

    std::string f2(std::string line) {
        std::string fileName = "";
        // Regular expression to capture contents of the "file" field
        std::regex rgx("\"file\": \"(.*)\"");
        std::smatch matches;

        if(std::regex_search(line, matches, rgx)) {
            if (matches.size() > 1){
                fileName = matches[1].str();
            }
        } else {
            std::cout << "Match not found\n";
        }
        return fileName;
    }
    std::string f3(std::string line) {
        std::string fileName = "";
        // Regular expression to capture contents of the "file" field
        std::regex rgx("\"directory\": \"(.*)\"");
        std::smatch matches;

        if(std::regex_search(line, matches, rgx)) {
            if (matches.size() > 1){
                fileName = matches[1].str();
            }
        } else {
            std::cout << "Match not found\n";
        }
        return fileName;
    }

    std::vector<CompileCommand> CompileCommandsReader::Read() {
        std::cout << "Reading include paths"<<std::endl;
        std::ifstream compileCommandsFile(compileCommandsFilePath);
        
        std::string line;
        std::regex isCommand("\"command\":");
        std::regex isFile("\"file\":");
        std::regex isDirectory("\"directory\":");

        std::vector<CompileCommand> cc;
    
        std::vector<std::string> includePaths;
        std::string directory = "";
        std::string sourceFile = "";
        
        bool isCommandFound = false;
        bool isFileFound = false;
        bool isDirectoryFound = false;

        while (std::getline(compileCommandsFile, line)) {
            if (std::regex_search(line, isCommand)) {
                    includePaths = f1(line);
                    if (isCommandFound == true) {
                        std::cout << "Failed on reading compile command" << std::endl;
                    }
                    isCommandFound = true;
                } else if (std::regex_search(line, isFile)) {
                    sourceFile =  f2(line);
                                        if (isFileFound == true) {
                        std::cout << "Failed on reading compile command" << std::endl;
                    }
                    isFileFound = true;

                }
                else if (std::regex_search(line, isDirectory)) {
                    directory =  f3(line);
                                                            if (isDirectoryFound == true) {
                        std::cout << "Failed on reading compile command" << std::endl;
                    }
                    isDirectoryFound = true;
                } 
                
                if (isCommandFound && isFileFound && isDirectoryFound) {
                    cc.emplace_back(directory, sourceFile, includePaths);
                    isCommandFound = false;
                    isFileFound = false;
                    isDirectoryFound = false;
                }
        }

        return cc;
    }
