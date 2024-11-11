#include <iostream>
#include "compile_commands_reader.hpp"
#include "source_file_reader.hpp"
#include "path_explorer.hpp"
#include "source_file.hpp"
#include <memory>
#include <filesystem>
#include <iostream>

#include <sys/stat.h>
#include <unistd.h>
#include <string>
#include <fstream>

bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}



void dfs (std::shared_ptr<SourceFile> node, std::string prefix = "") {
    if (node->GetIncludes().empty()) {
        std::cout << prefix << node->GetPath() << std::endl;
        return;
    }
    for (auto i : node->GetIncludes()) {
        dfs(i.lock(), prefix + node->GetPath() + " -> ");
    } 

}
class Report{
    public:
    static std::string Print(std::vector<std::shared_ptr<SourceFile>> & sourceFiles){
        for (auto i : sourceFiles) {
            std::cout << SourceFile::str(*i)<<std::endl;
            auto isRootNode =i->GetIncludedBy().empty();
            if (isRootNode) {
                dfs(i);
            }
        }
        return "";
    }

};

int main(int argc, char *argv[]) {

    Graph g;
    
    g.LinkNodes(g.AddNode("a"), g.AddNode("b"));
    g.LinkNodes(g.AddNode("b"), g.AddNode("c"));

    // g.LinkNodes(g.AddNode("a"), g.AddNode("b"));
    
    std::cout << g.Stats() << std::endl;

    g.ForEachNode([](Node & node){
        std::cout << node.Stringify() << std::endl;
    });

    // g.AddNode("b");
    // g.nodes["a"].setX(1);   
    // std::filesystem::path cwd = std::filesystem::current_path() // "filename.txt";
    // std::cout << std::filesystem::current_path() << std::endl;   

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
    graph.ForEachNode([](std::shared_ptr<SourceFile> node){
        std::cout <<"PATH: "<< node->GetPath() << std::endl;
    });
    auto x = graph.GetNodes();
    std::cout << "Items: "<< x.size() << std::endl;
    Report::Print(x);


    return 0;
}