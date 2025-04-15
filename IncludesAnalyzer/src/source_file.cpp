#include "source_file.hpp"
#include "path_explorer.hpp" // Assuming PathExplorer is defined here
#include <filesystem>

std::vector<std::string> ReadIncludes(std::string path) {
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

    std::vector<std::string> includers;
    includers.push_back(sourceFile);

    auto evalPath = [&includePaths](std::string include) {
        for (auto includePath : includePaths) {
            PathExplorer pe(includePath);
            if (pe.Contains(include)) {
                return includePath + "/" + include;
            }
        }
        return include;
    };

    while (!includers.empty()) {
        auto includer = includers.back();
        includers.pop_back();
        if (processedNodes.find(includer) != processedNodes.end()) {
            // std::cout << "Node " << includer << " already processed" << std::endl;
            continue;
        }

        auto short_includes = ReadIncludes(includer);
        auto includes = std::vector<std::string>();
        std::for_each(short_includes.begin(), short_includes.end(), [&includes, &evalPath](std::string &include) {
            includes.push_back(evalPath(include));
        });

        for (auto include : includes) {
            auto includerHandle = graph.AddNode(includer);
            auto includeeHandle = graph.AddNode(include);
            graph.LinkNodes(includerHandle, includeeHandle);
            includers.push_back(include);
        }
        processedNodes.insert(includer);
        if (includer == sourceFile) {
            source_files.push_back(includer);
            // auto sourceFileHandle = graph.GetNodeHandle(sourceFile);
            // if (sourceFileHandle.has_value()) {
                // graph.LinkNodes(sourceFileHandle.value(), includerHandle);
            // }
        }
    }
}

void FilesGraph::PrintStats() {
    graph.ForEachNode([](NodeHandle, Node &node) {
        std::cout << node.Stringify() << std::endl;
    });
}

std::vector<std::string> FilesGraph::GetDependenciesOf(std::string include) {
    std::vector<std::string> dependencies = {};
    auto handle = graph.GetNodeHandle(include);
    if (!handle.has_value()) {
        std::cout << "Node " << include << " not found" << std::endl;
        return dependencies;
    }

    auto listDependencies = [this, &handle, &dependencies](std::vector<NodeHandle> visited) {
        for (auto nodeHandle : visited) {
            if (nodeHandle == handle.value()) {
                //skip root node
                continue;
            }

            auto node = this->graph.GetNode(nodeHandle);
            dependencies.push_back(node.GetData());
            // std::cout << "Data: " << std::filesystem::path(node.GetData()).lexically_normal() << std::endl;
        }
        // std::cout << std::endl;
    };

    auto printPathDownward = [this, &handle, &dependencies](std::vector<NodeHandle> visited) {
        for (auto nodeHandle : visited) {
            auto node = this->graph.GetNode(nodeHandle);
            if (node.IsTerminal()) {
                std::cout << node.GetData();
            } else {
                std::cout << node.GetData() << " -> ";
            }
        }
        std::cout << std::endl;
    };
    
    graph.DfsDownwardNonRec(handle.value(), listDependencies);
    return dependencies;
}

void FilesGraph::GetUsersOf(std::string include) {
    auto handle = graph.GetNodeHandle(include);
    if (!handle.has_value()) {
        std::cout << "Node " << include << " not found" << std::endl;
        return;
    }

    auto printPathUpward = [this](std::vector<NodeHandle> visited) {
        for (auto nodeHandle : visited) {
            auto node = this->graph.GetNode(nodeHandle);
            if (node.IsRoot()) {
                std::cout << node.GetData();
            } else {
                std::cout << node.GetData() << " -> ";
            }
        }
        std::cout << std::endl;
    };
    graph.DfsUpward(handle.value(), printPathUpward);
}

void FilesGraph::PrintU() {
    auto printPathDownward = [this](std::vector<NodeHandle> visited) {
        for (auto nodeHandle : visited) {
            auto node = this->graph.GetNode(nodeHandle);
            if (node.IsRoot()) {
                std::cout << node.GetData();
            } else {
                std::cout << node.GetData() << " -> ";
            }
        }
        std::cout << std::endl;
    };

    graph.ForEachNode([this, &printPathDownward](NodeHandle handle, Node &node) {
        if (node.IsTerminal()) {
            this->graph.DfsUpward(handle, printPathDownward);
        }
    });
}

void FilesGraph::Print() {
    auto printPathDownward = [this](std::vector<NodeHandle> visited) {
        for (auto nodeHandle : visited) {
            auto node = this->graph.GetNode(nodeHandle);
            if (node.IsTerminal()) {
                std::cout << node.GetData();
            } else {
                std::cout << node.GetData() << " -> ";
            }
        }
        std::cout << std::endl;
    };

    graph.ForEachNode([this, &printPathDownward](NodeHandle handle, Node &node) {
        if (node.IsRoot()) {
            this->graph.DfsDownward(handle, printPathDownward);
        }
    });
}