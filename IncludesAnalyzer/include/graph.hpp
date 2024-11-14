#pragma once
#include <optional>
#include "common.hpp"
#include "node.hpp"
#include "edge.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>


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
        std::cout << "[Graph]: New node handle: "<<nodeHandle <<" data: " << data<< std::endl;
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
            std::cout << "[Graph]: Link "<<a<<" - "<<b <<" already exists" << std::endl;
            return;
        }


        if (nodes.find(from) == nodes.end() || nodes.find(to) == nodes.end()) {
            throw std::runtime_error("[Graph]: Node not found");
        }

        auto ptr = std::make_shared<Edge>(from, to);
        edges.emplace_back(ptr);
        

        auto it = nodes.find(from);
        if (it == nodes.end()) {
            throw std::runtime_error("[Graph]: Node not found");
        }
        it->second->CallbackEdgeFrom(ptr);
        
        it = nodes.find(to);
        if (it == nodes.end()) {
            throw std::runtime_error("[Graph]: Node not found");
        }
        it->second->CallbackEdgeTo(ptr);
    }

    Node & GetNode(NodeHandle handle)  {
        return *nodes[handle];
    }

    void ForEachNode(std::function<void(NodeHandle, Node &)> f) {
        for (auto i : nodes) {
            f(i.first, *i.second);
        }
    }
   
    bool Contains(NodeData data){
        return dataToHandle.find(data) != dataToHandle.end();
    }


    void DfsDownward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f){
        std::vector<NodeHandle> visited;
        visited.push_back(handle);
    
        auto node = nodes[handle];
    
        if (node->IsTerminal()) {
            f(visited);
            return;
        }
        
        for (auto i : node->NodesOutcoming()) {
            DfsInternalDownward(i, f, visited);
        }
    }


    void DfsUpward (NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f){
        std::vector<NodeHandle> visited;
        visited.push_back(handle);
    
        auto node = nodes[handle];
    
        if (node->IsRoot()) {
            f(visited);
            return;
        }
        
        for (auto i : node->NodesIncoming()) {
            DfsInternalUpward(i, f, visited);
        }
    }

    std::optional<NodeHandle> GetNodeHandle(NodeData data){
        if (dataToHandle.find(data) == dataToHandle.end()) {
            return std::nullopt;
        }
        return dataToHandle[data];
    }

    private:

    void DfsInternalDownward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, std::vector<NodeHandle> visited){
        auto node = nodes[handle];
        visited.push_back(handle);
        if (node->IsTerminal()) {
            f(visited);
            return;
        }

        for (auto i : node->NodesOutcoming()) {
            DfsInternalDownward(i,f, visited);
        }
    }
    void DfsInternalUpward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, std::vector<NodeHandle> visited){
        auto node = nodes[handle];
        visited.push_back(handle);
        if (node->IsRoot()) {
            f(visited);
            return;
        }

        for (auto i : node->NodesIncoming()) {
            DfsInternalUpward(i,f, visited);
        }
    }

    std::unordered_map<NodeData, NodeHandle> dataToHandle;
    std::unordered_map<NodeHandle, std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;

};