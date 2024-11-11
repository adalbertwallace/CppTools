#pragma once

using EdgeHandle = unsigned;
using NodeHandle = int;


class Edge{
    public:
        Edge(NodeHandle from, NodeHandle to): from(from), to(to){
        
        }

        NodeHandle GetFrom() const {
            return from;
        }
        NodeHandle GetTo() const {
            return to;
        }
    private:
        
        NodeHandle from;
        NodeHandle to;
};
