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
    Graph();
    ~Graph();
    
    enum class WalkDirection{
        upward,
        downward
    };

    NodeHandle AddNode(NodeData data);
    
    void LinkNodes(NodeHandle from, NodeHandle to);
    Node & GetNode(NodeHandle handle);
    
    void ForEachNode(std::function<void(NodeHandle, Node &)> f) ;
    void Walk(WalkDirection direction, NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f);

    private:
    
    void Walk(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, 
              std::function<bool(std::shared_ptr<Node>)> no_more_nodes,
              std::function<std::vector<NodeHandle>(std::shared_ptr<Node>)> get_next_nodes);
    private:
    std::string Stats();
    bool Contains(NodeData data);
    std::unordered_map<NodeData, NodeHandle> dataToHandle;
    std::unordered_map<NodeHandle, std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;

};