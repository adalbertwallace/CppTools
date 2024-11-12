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

    static std::string str(SourceFile& inc) {
        std::string r =  "item: " + inc.GetPath() + "\nincluded by:\n";
        for (auto i : inc.GetIncludedBy()) {
            r += i.lock()->GetPath() + "\n";
        }
        r += "includes:\n";
        for (auto i : inc.GetIncludes()) {
            r += i.lock()->GetPath() + "\n";
        }
                
        return r;
    }
    private:
        std::string path;
        std::vector<std::weak_ptr<SourceFile>> includedBy;
        std::vector<std::weak_ptr<SourceFile>> includes;
};
struct IncludeToVisit {
    public:
    std::vector<std::string> includePaths;
    std::string name;
    std::weak_ptr<SourceFile> includer;

    static std::string str(IncludeToVisit& inc) {
        std::string r =  inc.name + "included by " + inc.includer.lock()->GetPath() + " \npath set: ";
        for (auto i : inc.includePaths) {
            r += +"\n"+i + " ";
        }
        return r;
    }
};
class FilesGraph{
    private:
        Graph graph;
        std::set<std::string> processedNodes;
    public:
        FilesGraph(){
        }
        ~FilesGraph(){
        }
        
        bool UpdateNetwork2(CompileCommand & cc){
            
            std::cout << "Updating network"<<std::endl;

            auto sourceFile = SourceFile(cc.GetSourceFile());
            auto includerHandle = graph.AddNode(cc.GetSourceFile());
            processedNodes.insert(cc.GetSourceFile());

            auto includes = sourceFile.ReadIncludes();
            auto inclduePaths = cc.GetIncludePaths();

            bool terminatingNode = true;
            
            auto evalPath = [&inclduePaths, &terminatingNode](std::string include) {
                for (auto includePath : inclduePaths) {
                    PathExplorer pe(includePath);
                    if (pe.Contains(include)){
                        terminatingNode = false;
                        return includePath + "/" + include;
                    }
                }
                terminatingNode = true;
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
                    std::cout << "Now processing " << absolute_include_path << std::endl;
                    // if (processedNodes.find(absolute_include_path) != processedNodes.end()) {
                    //     continue;
                    // }
                    auto includes1 = SourceFile(absolute_include_path).ReadIncludes();
                    std::cout << "Includes number   "<< includes1.size() << std::endl;
                    for (auto i : includes1) {
                          std::cout << absolute_include_path << " includes " << i << std::endl;

                        includes.push_back(i);
                    }
                    processedNodes.insert(absolute_include_path);
                }


                // if (terminatingNode == false) {
                //     std::cout << "Now processing " << absolute_include_path << std::endl;
                //     // if (processedNodes.find(absolute_include_path) != processedNodes.end()) {
                //     //     continue;
                //     // }
                //     auto includes1 = SourceFile(absolute_include_path).ReadIncludes();
                //     std::cout << "Includes number   "<< includes1.size() << std::endl;
                //     for (auto i : includes1) {
                //           std::cout << absolute_include_path << " includes " << i << std::endl;

                //         includes.push_back(i);
                //     }
                // }

            }
            // for (auto include : includes) {
            //     auto absolute_include = evalPath(include);
            //     auto includeeHandle = graph.AddNode(absolute_include);
            //     graph.LinkNodes(includerHandle, includeeHandle);

            //     if (terminatingNode == false) {
            //         auto includes = SourceFile(absolute_include).ReadIncludes();
            //         for (auto i : includes) {
            //             includes.push_back(i);
            //         }
            //     }
            //     // std::string fileName = include;
            //     // bool terminatingNode = true;
            //     // for (auto includePath : cc.GetIncludePaths()) {
            //     //     PathExplorer pe(includePath);
            //     //     if (pe.Contains(include)){
            //     //         fileName = includePath + "/" + include;
            //     //         terminatingNode = false;
            //     //         break;
            //     //     }
            //     // }
            //     // if (graph.Contains(fileName)) {
            //     //     graph.LinkNodes(cc.GetSourceFile(), fileName);
            //     //     continue;
            //     // }
            //     // graph.AddNode(fileName);
            //     // graph.LinkNodes(cc.GetSourceFile(), fileName);
            //     // if (terminatingNode == false) {
            //     //     auto includes = SourceFile(fileName).ReadIncludes();
            //     //     for (auto i : includes) {
            //     //         includesToVisit.push_back({cc.GetIncludePaths(), i, fileName});
            //     //     }
            //     // }
            // }
            std::cout << graph.Stats() << std::endl;

    graph.ForEachNode([](Node & node){
        std::cout << node.Stringify() << std::endl;
     });

            return true;
            // std::vector<IncludeToVisit> includesToVisit;

            // for (auto include : includes) {
            //     std::string fileName = include;
            //     bool terminatingNode = true;
            //     for (auto includePath : cc.GetIncludePaths()) {
            //         PathExplorer pe(includePath);
            //         if (pe.Contains(include)){
            //             fileName = includePath + "/" + include;
            //             terminatingNode = false;
            //             break;
            //         }
            //     }
            //     if (graph.Contains(fileName)) {
            //         graph.LinkNodes(cc.GetSourceFile(), fileName);
            //         continue;
            //     }
            //     graph.AddNode(fileName);
            //     graph.LinkNodes(cc.GetSourceFile(), fileName);
            //     if (terminatingNode == false) {
            //         auto includes = SourceFile(fileName).ReadIncludes();
            //         for (auto i : includes) {
            //             includesToVisit.push_back({cc.GetIncludePaths(), i, fileName});
            //         }
            //     }
            // }
 


        }

        bool UpdateNetwork(CompileCommand & cc){
            // graph.AddNode(cc.GetSourceFile());

            // auto sourceFilePtr = std::make_shared<SourceFile>(cc.GetSourceFile());
            // auto includes = sourceFilePtr->ReadIncludes();
            // std::vector<IncludeToVisit> includesToVisit;
            // auto includes = sourceFilePtr -> ReadIncludes();
            // for (auto include : includes) {
            //     includesToVisit.push_back({cc.GetIncludePaths(), include, sourceFilePtr});
            // }

            


            auto sourceFilePtr = std::make_shared<SourceFile>(cc.GetSourceFile());
            nodes.push_back(sourceFilePtr);

            std::vector<IncludeToVisit> includesToVisit;
            auto includes = sourceFilePtr -> ReadIncludes();
            for (auto include : includes) {
                includesToVisit.push_back({cc.GetIncludePaths(), include, sourceFilePtr});
            }
                
            while (!includesToVisit.empty())
            {
                auto include = includesToVisit.back();
                includesToVisit.pop_back();
                // std::cout << " --> Visiting include " << IncludeToVisit::str(include) << std::endl;
        
                std::string fileName = include.name;
                bool terminatingNode = true;
                //resolve path
                for (auto includePath : include.includePaths) {
                    PathExplorer pe(includePath);
                    if (pe.Contains(include.name)){
                        fileName = includePath + "/" + include.name;
                        terminatingNode = false;
                        break;
                    } else {
                        std::cout << include.name << " not found in " << includePath << std::endl;
                    }
                }
           
            if (std::any_of(nodes.begin(), nodes.end(), [fileName, &include](std::shared_ptr<SourceFile> node){
                if (node->GetPath() == fileName) {
                    node->AddIncludedBy(include.includer);
                    include.includer.lock()->AddInclude(node);
                    return true;
                }
                return false;
             }))
             {
                 continue;
             }
            
            auto sourceFilePtr = std::make_shared<SourceFile>(fileName);
            sourceFilePtr->AddIncludedBy(include.includer);
            if (terminatingNode == false) {
                auto includes = sourceFilePtr->ReadIncludes();
                for (auto i : includes) {
                    includesToVisit.push_back({include.includePaths, i, sourceFilePtr});
                }
            
            }
        
        std::shared_ptr<SourceFile> spt = include.includer.lock();
        
        if (spt){
            spt->AddInclude(sourceFilePtr);
        }
        nodes.push_back(sourceFilePtr);
        
        }

        }

        void ForEachNode(std::function<void(std::shared_ptr<SourceFile>)> f) {
            for (auto i : nodes) {
                f(i);
            }
        }
                std::vector<std::shared_ptr<SourceFile>> GetNodes() const {
            return nodes;
        }

    private:
        std::vector<std::shared_ptr<SourceFile>> nodes;
};

