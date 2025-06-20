#include "source_file.hpp"
#include "path_explorer.hpp" // Assuming PathExplorer is defined here
#include <filesystem>
#include <sstream>

std::vector<std::string> ReadIncludes(std::string path) {
    // std::cout << "Reading includes for file: " << path << std::endl;
    std::ifstream fileStream(path.c_str());
    std::string line;
    std::regex includeRegex("^\\s*#include\\s*<(.*)>");
    std::regex includeRegex2("^\\s*#include\\s*\"(.*)\"");
    std::vector<std::string> includes;
    while (std::getline(fileStream, line)) {
        std::smatch match;
        if (std::regex_search(line, match, includeRegex)) {
            includes.push_back(match[1]);
        } else if (std::regex_search(line, match, includeRegex2)) {
            includes.push_back(match[1]);
        }
    }
    return includes;
}

FilesGraph::FilesGraph() {}

FilesGraph::~FilesGraph() {}

void FilesGraph::UpdateNetwork(CompileCommand &cc) {
    auto sourceFile = cc.GetSourceFile();
    auto includePaths = cc.GetIncludePaths();

    std::vector<std::string> files_to_process;
    files_to_process.push_back(sourceFile);

    auto pathEvaluator = [](std::vector<std::string> & includePaths){
        return [includePaths](std::string include){
            for (auto includePath : includePaths) {
                PathExplorer pe(includePath);
                if (pe.Contains(include)) {
                    return includePath + "/" + include;
                }
            }
            return include;
        };
    };

    while (!files_to_process.empty()) {
        auto file_being_processed = files_to_process.back();
        files_to_process.pop_back();
        
        if (fileToHandle.find(file_being_processed) != fileToHandle.end()) {
            continue;
        }

        auto short_includes = ReadIncludes(file_being_processed);
        auto includes = std::vector<std::string>();
        std::transform(short_includes.begin(), short_includes.end(), std::back_inserter(includes), pathEvaluator(includePaths));

        for (auto include : includes) {
           
            auto handle = graph.AddNode(file_being_processed);
            fileToHandle.insert({file_being_processed, handle});
            auto includeHandle = graph.AddNode(include);

            graph.LinkNodes(handle, includeHandle);
            files_to_process.push_back(include);
        }

        if (file_being_processed == sourceFile) {
            source_files.push_back(file_being_processed);
        }
    }
}

std::vector<std::string> FilesGraph::ListDependenciesOf(std::string file)
{
    return GetDependenciesOf(file, false);
}

std::vector<std::string> FilesGraph::ListChainOfDependenciesOf(std::string file){
    return GetDependenciesOf(file, true);
}


std::vector<std::string> FilesGraph::GetDependenciesOf(std::string include, bool displayChain) {
    std::vector<std::string> dependencies = {};
    auto it = fileToHandle.find(include);
    if ( it == fileToHandle.end()) {
        std::cout << "Node " << include << " not found" << std::endl;
        return dependencies;
    }
    NodeHandle handle = it->second;

    auto listDependencies = [this, &handle, &dependencies, &include](std::vector<NodeHandle> visited) {
        std::vector<std::string> deps;
        for (auto nodeHandle : visited) {
            if (nodeHandle == handle) {
                //skip root node
                continue;
            }
            auto node = this->graph.GetNode(nodeHandle);
            deps.push_back(node.GetData());
        }

        std::stringstream ss;
        ss << include <<" includes ";
        std::for_each(deps.begin(), deps.end(), [&](const std::string& dep){ 
            auto p = std::filesystem::path(dep);
            if (std::filesystem::exists(p)) {
                ss <<  std::string(std::filesystem::canonical(p)) << std::endl;
            } else {
                ss <<   std::string(p) << std::endl;
            }
        });
        dependencies.push_back(ss.str());
    };

    auto printPathDownward = [this, &handle, &dependencies](std::vector<NodeHandle> visited) {
        std::stringstream ss;

        for (auto nodeHandle : visited) {
            auto node = this->graph.GetNode(nodeHandle);
            if (node.IsTerminal()) {
                ss << node.GetData();
            } else {
                ss << node.GetData() << " includes ";
            }
        }
        ss << std::endl;
        dependencies.push_back(ss.str());
    };
    if (displayChain) {
        graph.Walk(Graph::WalkDirection::downward, handle, printPathDownward);
    } else {
        graph.Walk(Graph::WalkDirection::downward, handle, listDependencies);
    }
    return dependencies;
}
