#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <fstream>      // std::ifstream
#include <regex>
#include <memory>
#include<map>
#include "edge.hpp"
#include <sstream>
#include "graph.hpp"
#include "node.hpp"
#include "common.hpp"
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

        void UpdateNetwork(CompileCommand & cc){
            auto sourceFile = cc.GetSourceFile();
            auto inclduePaths = cc.GetIncludePaths();
            
            std::cout << sourceFile << std::endl;
            std::vector<std::string> includers;
            includers.push_back(sourceFile);

            auto evalPath = [&inclduePaths](std::string include) {
                for (auto includePath : inclduePaths) {
                    PathExplorer pe(includePath);
                    if (pe.Contains(include)){
                        return includePath + "/" + include;
                    }
                }
                return include;
            };

            while(!includers.empty()){
                auto includer = includers.back();
                includers.pop_back();
                if (processedNodes.find(includer) != processedNodes.end()) {
                    std::cout << "Node " << includer << " already processed" << std::endl;
                    continue;
                }

                auto short_includes = ReadIncludes(includer);
                auto includes = std::vector<std::string>();
                std::for_each(short_includes.begin(), short_includes.end(), [&includes, &evalPath](std::string& include){
                    includes.push_back(evalPath(include));
                });

                std::cout << "Analyzing file: " << includer << std::endl;
                for (auto include : includes) {
                    std::cout << "Include: " << include << std::endl;
                    auto includerHandle = graph.AddNode(includer);
                    auto includeeHandle = graph.AddNode(include);
                    graph.LinkNodes(includerHandle, includeeHandle);
                    includers.push_back(include);
                }
                processedNodes.insert(includer);

                
            }
        }

        void PrintStats(){
            graph.ForEachNode([](NodeHandle, Node & node){
                std::cout << node.Stringify() << std::endl;
            });
        }

        void Print(){

            auto printPathDownward = [this](std::vector<NodeHandle> visited){
            for (auto nodeHandle : visited) {
                auto node = this->graph.GetNode(nodeHandle);
                if (node.IsTerminal()) {
                    std::cout << node.GetData();
                } else {
                    std::cout << node.GetData() <<" -> ";
                }           
            }
            std::cout << std::endl;
            };

            graph.ForEachNode([this, &printPathDownward](NodeHandle handle, Node & node){
                if (node.IsRoot())
                {
                    this->graph.DfsDownward(handle, printPathDownward);
                }
            });
        }

        void GetDependenciesOf(std::string include){
            auto handle = graph.GetNodeHandle(include);
            if (!handle.has_value()) {
                std::cout << "Node " << include << " not found" << std::endl;
                return;
            }
            auto printPathDownward = [this](std::vector<NodeHandle> visited){
            for (auto nodeHandle : visited) {
                auto node = this->graph.GetNode(nodeHandle);
                if (node.IsTerminal()) {
                    std::cout << node.GetData();
                } else {
                    std::cout << node.GetData() <<" -> ";
                }           
            }
            std::cout << std::endl;
            };
            graph.DfsDownward(handle.value(), printPathDownward);
        }

        void GetUsersOf(std::string include){
            auto handle = graph.GetNodeHandle(include);
            if (!handle.has_value()) {
                std::cout << "Node " << include << " not found" << std::endl;
                return;
            }
            auto printPathUpward = [this](std::vector<NodeHandle> visited){
            for (auto nodeHandle : visited) {
                auto node = this->graph.GetNode(nodeHandle);
                if (node.IsRoot()) {
                    std::cout << node.GetData();
                } else {
                    std::cout << node.GetData() <<" -> ";
                }           
            }
            std::cout << std::endl;
            };
            graph.DfsUpward(handle.value(), printPathUpward);
        }

        void PrintU(){

            auto printPathDownward = [this](std::vector<NodeHandle> visited){
            for (auto nodeHandle : visited) {
                auto node = this->graph.GetNode(nodeHandle);
                if (node.IsRoot()) {
                    std::cout << node.GetData();
                } else {
                    std::cout << node.GetData() <<" -> ";
                }           
            }
            std::cout << std::endl;
            };

            graph.ForEachNode([this, &printPathDownward](NodeHandle handle, Node & node){
                if (node.IsTerminal())
                {
                    this->graph.DfsUpward(handle, printPathDownward);
                }
            });
        }
   private:
};

