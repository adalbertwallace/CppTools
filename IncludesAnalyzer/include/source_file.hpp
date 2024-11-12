#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>      // std::ifstream
#include <regex>
#include <memory>
#include<map>
#include "Edge.hpp"
#include <sstream>
#include "Graph.hpp"
#include "Node.hpp"
#include "Common.hpp"
#include <set>

std::vector<std::string> ReadIncludes(std::string path) 
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

class FilesGraph{
    private:
        Graph graph;
        std::set<std::string> processedNodes;
    public:
        FilesGraph(){
        }
        ~FilesGraph(){
        }
        
        bool UpdateNetwork(CompileCommand & cc){

            auto sourceFile = cc.GetSourceFile();
            auto includerHandle = graph.AddNode(cc.GetSourceFile());
            processedNodes.insert(cc.GetSourceFile());

            auto includes = ReadIncludes(sourceFile);
            auto inclduePaths = cc.GetIncludePaths();

            auto evalPath = [&inclduePaths](std::string include) {
                for (auto includePath : inclduePaths) {
                    PathExplorer pe(includePath);
                    if (pe.Contains(include)){
                        return includePath + "/" + include;
                    }
                }
                return include;
            };

            while(includes.empty() == false) {
                auto include = includes.back();
                includes.pop_back();

                auto absolute_include_path = evalPath(include);
                
                auto includeeHandle = graph.AddNode(absolute_include_path);
                graph.LinkNodes(includerHandle, includeeHandle);
                // processedNodes.insert(cc.GetSourceFile());

                if (processedNodes.find(absolute_include_path) != processedNodes.end()) {
                    continue;
                } else 
                {
                    // std::cout << "Now processing " << absolute_include_path << std::endl;
                    auto includes1 = ReadIncludes(absolute_include_path);
                    // std::cout << "Includes number   "<< includes1.size() << std::endl;
                    for (auto i : includes1) {
                          std::cout << absolute_include_path << " includes " << i << std::endl;

                        includes.push_back(i);
                    }
                    processedNodes.insert(absolute_include_path);
                }

            }
            
            std::cout << graph.Stats() << std::endl;

    graph.ForEachNode([](Node & node){
        std::cout << node.Stringify() << std::endl;
     });

            return true;
          
        }

   private:
};

