#include <gtest/gtest.h>
#include "graph.hpp"
#include "node.hpp"
#include "edge.hpp"

// Test fixture for the Graph class
class GraphTest : public ::testing::Test {
protected:
    Graph graph;

    void SetUp() override {
        // Setup code if needed
    }

    void TearDown() override {
        // Cleanup code if needed
    }
};

// Test: Adding a node to the graph
TEST_F(GraphTest, WalkDownward) {
    // 
    // A B D
    //     E
    //   C F H I
    //     G
    
    NodeHandle handleA = graph.AddNode("A");
    NodeHandle handleB = graph.AddNode("B");
    NodeHandle handleC = graph.AddNode("C");
    NodeHandle handleD = graph.AddNode("D");
    NodeHandle handleE = graph.AddNode("E");
    NodeHandle handleF = graph.AddNode("F");
    NodeHandle handleG = graph.AddNode("G");
    NodeHandle handleH = graph.AddNode("H");
    NodeHandle handleI = graph.AddNode("I");

    graph.LinkNodes(handleA, handleB);
    graph.LinkNodes(handleA, handleC);
    graph.LinkNodes(handleB, handleD);
    graph.LinkNodes(handleB, handleE);
    graph.LinkNodes(handleC, handleF);
    graph.LinkNodes(handleC, handleG);
    graph.LinkNodes(handleF, handleH);
    graph.LinkNodes(handleH, handleI);

    std::vector<std::vector<NodeHandle>> paths;
    std::stringstream ss;
    graph.Walk(Graph::WalkDirection::downward, handleA, [&paths](std::vector<NodeHandle> visited) {
        paths.push_back(visited);
    });
    
    for (auto path : paths) {
        ss << "Path: ";
        for (auto node : path) {
            ss << graph.GetNode(node).GetData() << " ";
        }
        ss << std::endl;
    }

    std::stringstream expected;
    expected << "Path: A C G "<<std::endl;
    expected << "Path: A C F H I "<<std::endl;
    expected << "Path: A B E "<<std::endl;
    expected << "Path: A B D "<<std::endl;

    EXPECT_EQ(ss.str(), expected.str());
}

TEST_F(GraphTest, WalkUpward) {
    // 
    // A B D
    //     E
    //   C F H I
    //     G
    
    NodeHandle handleA = graph.AddNode("A");
    NodeHandle handleB = graph.AddNode("B");
    NodeHandle handleC = graph.AddNode("C");
    NodeHandle handleD = graph.AddNode("D");
    NodeHandle handleE = graph.AddNode("E");
    NodeHandle handleF = graph.AddNode("F");
    NodeHandle handleG = graph.AddNode("G");
    NodeHandle handleH = graph.AddNode("H");
    NodeHandle handleI = graph.AddNode("I");

    graph.LinkNodes(handleA, handleB);
    graph.LinkNodes(handleA, handleC);
    graph.LinkNodes(handleB, handleD);
    graph.LinkNodes(handleB, handleE);
    graph.LinkNodes(handleC, handleF);
    graph.LinkNodes(handleC, handleG);
    graph.LinkNodes(handleF, handleH);
    graph.LinkNodes(handleH, handleI);

    std::vector<std::vector<NodeHandle>> paths;
    std::stringstream ss;
    graph.Walk(Graph::WalkDirection::upward, handleI, [&paths](std::vector<NodeHandle> visited) {
        paths.push_back(visited);
    });

    for (auto path : paths) {
        ss << "Path: ";
        for (auto node : path) {
            ss << graph.GetNode(node).GetData() << " ";
        }
        ss << std::endl;
    }

    std::stringstream expected;
    expected << "Path: I H F C A "<<std::endl;

    EXPECT_EQ(ss.str(), expected.str());
}

