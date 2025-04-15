#pragma once
#include <memory>
#include <vector>
#include <sstream>
#include "common.hpp"
#include "edge.hpp"

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

        bool IsRoot() const {
            return edgesTo.empty();
        }

        bool IsTerminal() const {
            return edgesFrom.empty();
        }   
        //Get nodes that originates edges to this node
        std::vector<NodeHandle> NodesIncoming() const {
            std::vector<NodeHandle> result;
            for (auto edge : edgesTo) {
                result.push_back(edge.lock()->GetFrom());
            }
            return result;
        }

        //Get nodes that this node originates edges to
        std::vector<NodeHandle> NodesOutcoming() const {
            std::vector<NodeHandle> result;
            for (auto edge : edgesFrom) {
                result.push_back(edge.lock()->GetTo());
            }
            return result;
        }
        std::string Stringify() const {
            std::stringstream ss;
            ss << "Node data: " << std::endl;
            ss << data << std::endl;
            ss << "Edges from: " ;
            for (auto i : edgesFrom) {
                ss << std::endl;
                ss << i.lock()->GetFrom() << " - " << i.lock()->GetTo();
            }
            ss << std::endl;

            ss << "Edges to: ";
            for (auto i : edgesTo) {
                ss << std::endl;
                ss << i.lock()->GetFrom() << " - " << i.lock()->GetTo();
            }

            return ss.str();
        }

    private:
        NodeData data;
        std::vector<std::weak_ptr<Edge>> edgesFrom; // All edges that originate from this node
        std::vector<std::weak_ptr<Edge>> edgesTo; // All edges that end in this node
};

