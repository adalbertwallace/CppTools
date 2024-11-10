#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>      // std::ifstream
#include <regex>
#include <memory>


using NodeHandle = int;



class SourceFile{
    public:
        SourceFile(const std::string& aboslutePath):  path(aboslutePath){
            // std::cout << "SourceFile::SourceFile " << path << std::endl;
            // ;
        }
        ~SourceFile(){
            // std::cout << "SourceFile::~SourceFile " << path << std::endl;
        }
    
        std::vector<std::string> ReadIncludes() const
        {
        std::ifstream fileStream(path.c_str());
        std::string line;
        std::regex includeRegex("^\\s*#include\\s*<(.*)>");
        std::regex includeRegex2("^\\s*#include\\s*\"(.*)\"");
        std::vector<std::string> includes;
        while (std::getline(fileStream, line)) {
            std::smatch match;
            if (std::regex_search(line, match, includeRegex)) {
                includes.push_back(match[1]);
            } else if (std::regex_search(line, match, includeRegex2)) {
                includes.push_back(match[1]);
            }
        }
        return includes;
  
        }

        std::string GetPath() const {
            return path;
        }
        void AddIncludedBy(std::weak_ptr<SourceFile> includedBy){
            this->includedBy.push_back(includedBy);
        }
        void AddInclude(std::weak_ptr<SourceFile> include){
            this->includes.push_back(include);
        }
        std::vector<std::weak_ptr<SourceFile>> GetIncludedBy() const {
            return includedBy;
        }
        std::vector<std::weak_ptr<SourceFile>> GetIncludes() const {
            return includes;
        }
    private:
        std::string path;
        std::vector<std::weak_ptr<SourceFile>> includedBy;
        std::vector<std::weak_ptr<SourceFile>> includes;
};

class FilesGraph{
    public:
        FilesGraph(){
        }
        ~FilesGraph(){
        }

        std::shared_ptr<SourceFile> AddNode(std::string absoluteFile){
            // auto node = std::make_shared<SourceFile>(absoluteFile);
            // nodes.push_back(node);
            return nullptr;
        }

        void AddIncludeRelation(std::shared_ptr<SourceFile> from, std::shared_ptr<SourceFile> to){
            // from->AddInclude(to);
            // to->AddIncludedBy(from);
        }

        std::vector<std::shared_ptr<SourceFile>> GetNodes() const {
            return nodes;
        }

    private:
        std::vector<std::shared_ptr<SourceFile>> nodes;
};