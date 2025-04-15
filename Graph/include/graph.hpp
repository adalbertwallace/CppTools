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
    
    NodeHandle AddNode(NodeData data);
    
    void LinkNodes(NodeHandle from, NodeHandle to);
    Node & GetNode(NodeHandle handle);
    
    void ForEachNode(std::function<void(NodeHandle, Node &)> f) ;
    
    
    void DfsDownward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f);
    void DfsDownwardNonRec(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f);
    void DfsUpward (NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f);
    std::optional<NodeHandle> GetNodeHandle(NodeData data);
    private:
    void DfsInternalDownward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, std::vector<NodeHandle> visited);
    void DfsInternalUpward(NodeHandle handle, std::function<void(std::vector<NodeHandle>)> f, std::vector<NodeHandle> visited);
    private:
    std::string Stats();
    bool Contains(NodeData data);
    std::unordered_map<NodeData, NodeHandle> dataToHandle;
    std::unordered_map<NodeHandle, std::shared_ptr<Node>> nodes;
    std::vector<std::shared_ptr<Edge>> edges;

};