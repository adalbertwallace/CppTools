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

struct IncludeToVisit {
    public:
    std::vector<std::string> includePaths;
    std::string name;
    std::weak_ptr<SourceFile> includedBy;

    static std::string str(IncludeToVisit& inc) {
        std::string r =  inc.name + " included by " + inc.includedBy.lock()->GetPath() + " \npath set: ";
        for (auto i : inc.includePaths) {
            r += +"\n"+i + " ";
        }
        return r;
    }
};


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
            auto isRootNode =i->GetIncludedBy().empty();
            if (isRootNode) {
                dfs(i);
            }
        }
        return "";
    }

};

int main(int argc, char *argv[]) {

    FilesGraph graph;

    // std::filesystem::path cwd = std::filesystem::current_path() // "filename.txt";
    std::cout << std::filesystem::current_path() << std::endl;   

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
    
    std::vector<std::shared_ptr<SourceFile>> sourceFiles;
    std::vector<IncludeToVisit> includesToVisit;


    for (auto cc : ccs) {
        auto sourceFilePtr = std::make_shared<SourceFile>(cc.GetSourceFile());
        sourceFiles.push_back(sourceFilePtr);
        auto includes = sourceFiles.back()->ReadIncludes();
        for (auto include : includes) {
            includesToVisit.push_back({cc.GetIncludePaths(), include, sourceFilePtr});
        }
    }
    // std::cout << "All basic commands processed" << std::endl;
    // for (std::shared_ptr<SourceFile> i : sourceFiles) {
    //     std::cout << "Source file: " << i << std::endl;
    // }

    // std::cout << "==== Includes to visit ==== "<<std::endl;
    // for (auto i : includesToVisit) {
    //     std::cout << IncludeToVisit::str(i) << std::endl;
    // }
    // std::cout << "==== Includes to visit ==== "<<std::endl;

    while (!includesToVisit.empty())
    {
        // std::cout << "New loop: includesToVisit.size(): " << includesToVisit.size() <<std::endl;
        auto include = includesToVisit.back();
        includesToVisit.pop_back();
        // std::cout << " --> Visiting include " << IncludeToVisit::str(include) << std::endl;
        
        std::string fileName = include.name;
        bool terminatingNode = true;
        //resolve path
        for (auto includePath : include.includePaths) {
            PathExplorer pe(includePath);
            if (pe.Contains(include.name)){
                fileName = includePath + "/" + include.name;
                terminatingNode = false;
                break;
            } else {
                std::cout << include.name << " not found in " << includePath << std::endl;
            }
        }

        auto sourceFilePtr = std::make_shared<SourceFile>(fileName);
        sourceFilePtr->AddIncludedBy(include.includedBy);
        if (terminatingNode == false) {
            auto includes = sourceFilePtr->ReadIncludes();
            for (auto i : includes) {
                includesToVisit.push_back({include.includePaths, i, sourceFilePtr});
            }
            
        }
        std::shared_ptr<SourceFile> spt = include.includedBy.lock();
        
        if (spt){
            spt->AddInclude(sourceFilePtr);
        }
        sourceFiles.push_back(sourceFilePtr);
        
    }
    
    Report::Print(sourceFiles);
    return 0;
}