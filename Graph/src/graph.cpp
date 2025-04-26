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

    void Graph::Walk(WalkDirection direction, NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f)
    {
        if (direction == WalkDirection::upward) {
            Walk(handle, f, [](std::shared_ptr<Node> node ){
                return node->IsRoot();
            }, [](std::shared_ptr<Node> node ){
                return node->NodesIncoming();
            });
        } else if (direction == WalkDirection::downward) {
            Walk(handle, f, [](std::shared_ptr<Node> node ){
                return node->IsTerminal();
            }, [](std::shared_ptr<Node> node ){
                return node->NodesOutcoming();
            });
        } else {
            throw std::runtime_error("[Graph]: Unknown walk direction");
        }
    }
    void Graph::Walk(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f
        , std::function<bool(std::shared_ptr<Node>)> no_more_nodes,
        std::function<std::vector<NodeHandle>(std::shared_ptr<Node>)> get_next_nodes) {
        

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

            if (no_more_nodes(node)) {
                std::vector<NodeHandle> p = {};
                for (auto i : visited) {
                    p.push_back(i.first);
                }
                f(p);
                visited.pop_back();
                continue;
            }

            for (auto nextHandle : get_next_nodes(node)) {
                stack.push_back({nextHandle, currentDepth + 1});
            }
        }
    }
