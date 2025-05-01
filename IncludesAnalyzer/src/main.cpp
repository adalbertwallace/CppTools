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

int main(int argc, char *argv[]) {

    std::string map_from = "/home/wallace/Workspace/CppTools_impl_in_cpp/sampleProject";
    std::string map_to = "/home/wallace/Workspace/CppTools_impl_in_cpp/sampleProject2";

    if (argc < 2) {
        std::cerr << "Error: Path to compile commands file not provided." << std::endl;
        return 1;
    }
    
    if (file_exists(argv[1]) == false) {
        std::cerr << "Error: File "<<argv[1]<<" does not exist." << std::endl;
        return 1;
    }
    std::filesystem::path compileCommandsFilePath = std::filesystem::path(argv[1]);
    
    auto r = CompileCommandsReader(compileCommandsFilePath);
    auto ccs = r.Read();

    std::for_each(ccs.begin(), ccs.end(), [&map_from, &map_to](CompileCommand& cc){
        std::cout << "Before mapping: " << cc.ToString() << std::endl;
        cc.ApplyMapping(map_from, map_to);
        std::cout << "After mapping: " << cc.ToString() << std::endl;
    });

    FilesGraph graph;
    for (auto cc : ccs) {
        std::cout <<"Processing compile command for file: "<< cc.GetSourceFile() << std::endl;
        graph.UpdateNetwork(cc);
    }

    std::cout << "Dependencies: "<<std::endl;

    const size_t sources_count = graph.GetSources().size();
    size_t current_source_idx = 1;
    for (auto src : graph.GetSources()){
           auto deps = graph.GetDependenciesOf(src);
           
           std::for_each(deps.begin(), deps.end(), [&src, &sources_count, &current_source_idx](std::string dep){
               std::cout <<"File["<<current_source_idx<<" of " << sources_count << "]: " <<  src << " depends on " <<dep<< std::endl;
           });
           current_source_idx++;
    }


    return 0;
}