#pragma once
#include <memory>
#include <vector>
#include <sstream>
#include "Common.hpp"
#include "Edge.hpp"

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

