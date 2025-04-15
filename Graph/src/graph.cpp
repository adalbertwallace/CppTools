#include <optional>
#include "common.hpp"
#include "node.hpp"
#include "edge.hpp"
#include "graph.hpp"
#include <unordered_map>
#include <memory>
#include <vector>
#include <functional>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <stack>

    Graph::Graph(){
        currentNode = 1;
    }

    Graph::~Graph(){
    }

    std::string Graph::Stats(){
        std::stringstream ss;
        ss << "Graph stats: " << std::endl;
        ss << "Nodes: " << nodes.size() << std::endl;
        ss << "Edges: " << edges.size() << std::endl;
        return ss.str();
    }

    NodeHandle Graph::AddNode(NodeData data){
        if (dataToHandle.find(data) != dataToHandle.end()) {
            return dataToHandle[data];
        }

        nodes.insert({currentNode, std::make_shared<Node>(data)});
        dataToHandle.insert({data, currentNode});
        auto nodeHandle = currentNode;
        currentNode++;
        return nodeHandle;
    }

    void Graph::LinkNodes(NodeHandle from, NodeHandle to){
        
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

    Node & Graph::GetNode(NodeHandle handle)  {
        return *nodes[handle];
    }

    void Graph::ForEachNode(std::function<void(NodeHandle, Node &)> f) {
        for (auto i : nodes) {
            f(i.first, *i.second);
        }
    }
   
    bool Graph::Contains(NodeData data){
        return dataToHandle.find(data) != dataToHandle.end();
    }


    void Graph::DfsDownwardNonRec(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f){
        size_t depth = 0;

        std::vector<std::pair<NodeHandle,size_t>> stack;
        std::vector<std::pair<NodeHandle,size_t>> visited;

        stack.push_back({handle, depth});
        depth++;

        while (!stack.empty()) {
            auto [currentHandle, currentDepth] = stack.back();
            stack.pop_back();
            
            while (visited.size() > 0 && visited.back().second >= currentDepth) {
                visited.pop_back();
            }

            visited.push_back({currentHandle, currentDepth});            
            
            auto node = nodes[currentHandle];

            if (node->IsTerminal()) {
                
                std::vector<NodeHandle> p = {};
                for (auto i : visited) {
                    p.push_back(i.first);
                }
                f(p);
                visited.pop_back();
                continue;
            }

            for (auto nextHandle : node->NodesOutcoming()) {
                stack.push_back({nextHandle, currentDepth + 1});
            }
        }
    }

    void Graph::DfsDownward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f){
        std::vector<NodeHandle> visited={};
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


    void Graph::DfsUpward (NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f){
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

    std::optional<NodeHandle> Graph::GetNodeHandle(NodeData data){
        if (dataToHandle.find(data) == dataToHandle.end()) {
            return std::nullopt;
        }
        return dataToHandle[data];
    }

    void Graph::DfsInternalDownward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, std::vector<NodeHandle> visited){
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
    void Graph::DfsInternalUpward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, std::vector<NodeHandle> visited){
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
