#include <iostream>
#include "compile_commands_reader.hpp"
#include "path_explorer.hpp"
#include "source_file.hpp"
#include <memory>
#include <filesystem>
#include <iostream>
#include "graph.hpp"
#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

void test(){
    Graph graph;
    auto a = graph.AddNode("a");
    auto b = graph.AddNode("b");
    auto c = graph.AddNode("c");
    auto d = graph.AddNode("d");
    auto e = graph.AddNode("e");
    auto f = graph.AddNode("f");
    auto g = graph.AddNode("g");
    auto h = graph.AddNode("h");
    auto i = graph.AddNode("i");

    graph.LinkNodes(a,b);
    graph.LinkNodes(a,d);
    graph.LinkNodes(b,c);
    graph.LinkNodes(d,e);
    graph.LinkNodes(d,f);
    graph.LinkNodes(f,h);
    graph.LinkNodes(f,g);
    graph.LinkNodes(e,i);
    graph.LinkNodes(g,i);

    // g.LinkNodes(g.AddNode("a"), g.AddNode("b"));
    std::cout << "===" << std::endl;
    std::cout << graph.Stats() << std::endl;
    std::cout << "===" << std::endl;

    auto printPathDownward = [&graph](std::vector<NodeHandle> visited){
        for (auto nodeHandle : visited) {
            auto node = graph.GetNode(nodeHandle);
            if (node.IsTerminal()) {
                std::cout << node.GetData();
            } else {
                std::cout << node.GetData() <<" -> ";
            }           
        }
        std::cout << std::endl;
    };

    auto printPathUpward = [&graph](std::vector<NodeHandle> visited){
        for (auto nodeHandle : visited) {
            auto node = graph.GetNode(nodeHandle);
            if (node.IsRoot()) {
                std::cout << node.GetData();
            } else {
                std::cout << node.GetData() <<" -> ";
            }           
        }
        std::cout << std::endl;
    };


    graph.DfsDownward(a, printPathDownward);
    graph.DfsUpward(i, printPathUpward);


}
int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Error: Path to compile commands file not provided." << std::endl;
        return 1;
    }
    
    if (file_exists(argv[1]) == false) {
        std::cerr << "Error: File "<<argv[1]<<" does not exist." << std::endl;
        return 1;
    }
    std::string compileCommandsFilePath = argv[1];
    
    auto r = CompileCommandsReader(compileCommandsFilePath);
    auto ccs = r.Read();
    
    FilesGraph graph;
    for (auto cc : ccs) {
        graph.UpdateNetwork(cc);
    }

    graph.Print();
    std::cout << "===" << std::endl;
    graph.PrintU();
    std::cout << "===" << std::endl;

    graph.GetDependenciesOf("/home/wallace/Workspace/CppTools/sampleProject/src/main.cpp");
    std::cout << "===" << std::endl;
    graph.GetUsersOf("/home/wallace/Workspace/CppTools/sampleProject/build/../include/main_inc1_helper.hpp");

    return 0;
}