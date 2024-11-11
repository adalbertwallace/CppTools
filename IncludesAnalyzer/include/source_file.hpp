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
    public:
        FilesGraph(){
        }
        ~FilesGraph(){
        }

        bool UpdateNetwork(CompileCommand & cc){
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

using NodeData = std::string;

class Node{
    public:
        Node(NodeData data) : data(data){
        }
        void CallbackEdgeTo(std::shared_ptr<Edge> edge){
            edgesTo.push_back(edge);
        }
        void CallbackEdgeFrom(std::shared_ptr<Edge> edge){
            edgesFrom.push_back(edge);
        }

        NodeData GetData() const {
            return data;
        }

        std::string Stringify() const {
            std::stringstream ss;
            ss << "Node data: " << std::endl;
            ss << data << std::endl;
            ss << "Edges from: " << std::endl;
            for (auto i : edgesFrom) {
                ss << i.lock()->GetFrom() << " - " << i.lock()->GetTo() << std::endl;;
            }
            ss << "Edges to: " << std::endl;
            for (auto i : edgesTo) {
                ss << i.lock()->GetFrom() << " - " << i.lock()->GetTo() << std::endl;;
            }
            return ss.str();
        }

    private:
        NodeData data;
        std::vector<std::weak_ptr<Edge>> edgesFrom;
        std::vector<std::weak_ptr<Edge>> edgesTo;
};



class Graph{

private:
    NodeHandle currentNode;
public:
    Graph(){
        currentNode = 1;
    }

    ~Graph(){
    }

    std::string Stats(){
        std::stringstream ss;
        ss << "Graph stats: " << std::endl;
        ss << "Nodes: " << nodes.size() << std::endl;
        ss << "Edges: " << edges.size() << std::endl;
        return ss.str();
    }

    NodeHandle AddNode(NodeData data){

        if (dataToHandle.find(data) != dataToHandle.end()) {
            return dataToHandle[data];
        }

        nodes.insert({currentNode, std::make_shared<Node>(data)});
        dataToHandle.insert({data, currentNode});
        auto nodeHandle = currentNode;
        std::cout << "Node " << data << " added with handle " << nodeHandle << std::endl;
        currentNode++;
        return nodeHandle;
    }

    void LinkNodes(NodeHandle from, NodeHandle to){
        
        if (std::any_of(edges.begin(), edges.end(), 
            [from, to](std::shared_ptr<Edge> edge)
        {
            if (edge->GetFrom() == from && edge->GetTo() == to) {
                return true;
            }
            return false;
        }))
        {
            auto a = nodes.find(from)->second->GetData();
            auto b = nodes.find(to)->second->GetData();
            std::cout << "Link "<<a<<" - "<<b <<" already exists" << std::endl;
            return;
        }


        if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end()) {
            throw std::runtime_error("Node not found");
        }

        auto ptr = std::make_shared<Edge>(from, to);
        edges.emplace_back(ptr);
        

        auto it = nodes.find(from);
        if (it == nodes.end()) {
            throw std::runtime_error("Node not found");
        }
        it->second->CallbackEdgeFrom(ptr);
        
        it = nodes.find(to);
        if (it == nodes.end()) {
            throw std::runtime_error("Node not found");
        }
        it->second->CallbackEdgeTo(ptr);
    }

    void ForEachNode(std::function<void(Node &)> f) {
        for (auto i : nodes) {
            f(*i.second);
        }
    }

    std::unordered_map<NodeData, NodeHandle> dataToHandle;
    std::unordered_map<NodeHandle, std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
    private:

};