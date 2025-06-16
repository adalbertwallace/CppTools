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
#include <vector> // Added for argument parsing
#include <optional> // Added for optional mapping

bool file_exists(const std::string& name) {
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}

void print_usage(const char* program_name) {
    std::cerr << "Usage: " << program_name << " --compile_commands_file <path> [--mapping <map_from> <map_to>]" << std::endl;
}

int main(int argc, char *argv[]) {

    std::string compile_commands_path_str;
    std::optional<std::pair<std::string, std::string>> mapping;

    // --- Argument Parsing Logic ---
    std::vector<std::string> args(argv + 1, argv + argc); // Convert C-style args to vector of strings

    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--compile_commands_file") {
            if (i + 1 < args.size()) {
                compile_commands_path_str = args[i + 1];
                i++; // Skip the value
            } else {
                std::cerr << "Error: --compile_commands_file requires a value." << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        } else if (args[i] == "--mapping") {
            if (i + 2 < args.size()) {
                mapping = std::make_pair(args[i + 1], args[i + 2]);
                i += 2; // Skip the two values
            } else {
                std::cerr << "Error: --mapping requires two values (<map_from> <map_to>)." << std::endl;
                print_usage(argv[0]);
                return 1;
            }
        } else {
            std::cerr << "Error: Unknown argument: " << args[i] << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    // --- Validation ---
    if (compile_commands_path_str.empty()) {
        std::cerr << "Error: Mandatory parameter --compile_commands_file is missing." << std::endl;
        print_usage(argv[0]);
        return 1;
    }

    if (!file_exists(compile_commands_path_str)) {
        std::cerr << "Error: File " << compile_commands_path_str << " does not exist." << std::endl;
        return 1;
    }
    // --- End Argument Parsing and Validation ---


    std::filesystem::path compileCommandsFilePath = std::filesystem::path(compile_commands_path_str);

    auto r = CompileCommandsReader(compileCommandsFilePath);
    auto ccs = r.Read();

    // Apply mapping if provided
    if (mapping.has_value()) {
        const auto& [map_from, map_to] = mapping.value();
        std::cout << "Applying mapping: " << map_from << " -> " << map_to << std::endl;
        std::for_each(ccs.begin(), ccs.end(), [&](CompileCommand& cc){
            std::cout << "Before mapping: " << cc.ToString() << std::endl; // Optional: Keep for debugging
            cc.ApplyMapping(map_from, map_to);
            std::cout << "After mapping: " << cc.ToString() << std::endl; // Optional: Keep for debugging
        });
    }

    FilesGraph graph;
    for (auto& cc : ccs) { // Use reference to avoid copying
        std::cout <<"Processing compile command for file: "<< cc.GetSourceFile() << std::endl;
        graph.UpdateNetwork(cc);
    }

    std::cout << "Dependencies: "<<std::endl;

    const auto sources = graph.GetSources(); // Get sources once
    const size_t sources_count = sources.size();
    size_t current_source_idx = 1;
    for (const auto& src : sources){ // Use const reference
           auto deps = graph.ListChainOfDependenciesOf(src);
           std::for_each(deps.begin(), deps.end(), [&](const std::string& dep){ 
            std::cout <<"File["<<current_source_idx<<" of " << sources_count << "]: "<<  dep;
           });
            // std::filesystem::path().absolute(src);
        //    std::for_each(deps.begin(), deps.end(), [&](const std::string& dep){ // Use const reference
        //     auto p = std::filesystem::path(dep);
            
        //     if (std::filesystem::exists(p)) {
        //         std::cout <<"File["<<current_source_idx<<" of " << sources_count << "]: " <<  src << " depends on " << std::filesystem::canonical(p) << std::endl;
        //     } else {
        //         std::cout <<"File["<<current_source_idx<<" of " << sources_count << "]: " <<  src << " depends on " << p << std::endl;
        //     }
            
        //    });
           current_source_idx++;
    }


    return 0;
}