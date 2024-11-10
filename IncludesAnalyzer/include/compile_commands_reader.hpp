#pragma once
#include <iostream>
#include <vector>
#include "compile_command.hpp"
class CompileCommandsReader{
public:
    CompileCommandsReader(const std::string& compile_commands_path):
    compileCommandsFilePath(compile_commands_path){
        ;
    }
    std::vector<CompileCommand> Read();
private:
    std::string compileCommandsFilePath;
};