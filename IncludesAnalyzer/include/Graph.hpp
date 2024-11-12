#pragma once

#include "Common.hpp"
#include "Node.hpp"
#include "Edge.hpp"
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

    bool Contains(NodeData data){
        return dataToHandle.find(data) != dataToHandle.end();
    }

    std::unordered_map<NodeData, NodeHandle> dataToHandle;
    std::unordered_map<NodeHandle, std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;
    private:

};