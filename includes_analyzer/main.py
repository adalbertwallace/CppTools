import argparse
import os
import json
import re

class Mapping:
    def init(self, str_from, str_to):
        self.str_from = str_from
        self.str_to = str_to
    

class CompileCommand:
    @classmethod
    def parse_compile_commands(cls, compile_commands_path, mapping = None):
        with open(compile_commands_path, 'r') as file:
            compile_commands = json.load(file)
        

        parsed_commands = []
        compile_command_id = 1
        for command in compile_commands:

            if mapping is not None:
                command['directory'] = command['directory'].replace(mapping.str_from, mapping.str_to)
                command['command'] = command['command'].replace(mapping.str_from, mapping.str_to)
                command['file'] = command['file'].replace(mapping.str_from, mapping.str_to)

            parsed_commands.append(cls(compile_command_id,compile_commands_path, command['directory'], command['command'], command['file']))
            compile_command_id+=1
        
        return parsed_commands

    def __init__(self,id, compile_commands_path, directory, command, file):
        self.compile_commands_path = compile_commands_path
        self.directory = directory
        self.command = command
        self.file = file
        self.id = id
    def get_include_directories(self):
        compile_commands_path = os.path.dirname(self.compile_commands_path)
        
        dirs = self._extract_include_directories(self.command)
        for i in range(len(dirs)):
            if not os.path.isabs(dirs[i]):
                dirs[i] = os.path.join(compile_commands_path, self.directory, dirs[i])
        return dirs
    
    def _extract_include_directories(self, input_string):
        # Define the regex pattern to match strings starting with " -I" and ending with the first whitespace character
        pattern = r' -I\S*'
        # Find all occurrences of the pattern in the input string
        matches = re.findall(pattern, input_string)
        for i in range(len(matches)):
            # Remove the leading " -I" from each match
            matches[i] = matches[i][3:]
        return matches

class SourceFile:
    def __init__(self, path, include_directories):
        self.path = path
        if not os.path.exists(path):
            print(f'Error: file {path} not found')
            exit(1)
        self.include_directories = include_directories
        # self.includes = self._parse_includes()
        self.include_chains = []

    def get_include_chains(self):
        self._parse_includes()
        return self.include_chains
    
    def _parse_includes(self):
        includes = self._find_includes(self.path)

        for include in includes:
            include_abs_path = self._get_full_path(include)
            if include_abs_path is None:
                print(f'Error: include {include} not found in {self.path}')
                continue
            self._parse_includes_internal(include_abs_path, self.path)

        return includes
    
    def _find_includes(self, file_path):
        if not os.path.exists(file_path):
            return []

        include_pattern = re.compile(r'#include\s*["<](.*?)[">]')
        includes = []
        with open(file_path, 'r') as file:
            for line in file:
                match = include_pattern.search(line)
                if match:
                    includes.append(match.group(1))
        return includes
    
    def _get_full_path(self, current_file_path):
        
        for directory in self.include_directories:
            include_path = os.path.join(directory, current_file_path)
            if os.path.exists(include_path):
                return include_path
        return current_file_path
    
    def _parse_includes_internal(self, current_file_path, parent, log=""):
        log += f'{parent} -> '

        includes = self._find_includes(current_file_path)
        if len(includes) == 0:
            log += f'{current_file_path}'
            self.include_chains.append(log)
            return
        
        for include in includes:
            self._parse_includes_internal(self._get_full_path(include), current_file_path, log)
        pass


def get_valid_compile_commands_path(path):

    compile_commands_path = os.path.abspath(path)
    name = 'compile_commands.json'
    if os.path.isdir(compile_commands_path):
        for root, dirs, files in os.walk(compile_commands_path):
            if name in files:
                compile_commands_path = os.path.join(root, name)
                break
    
    if not os.path.exists(compile_commands_path):
        print(f'Error: path {compile_commands_path} do not exist')
        return None
    
    return compile_commands_path
    

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--compile_commands', help='Path to compile_commands.json file or directory')
    parser.add_argument('--mapping', nargs=2, help='Two string values for mapping')
    args = parser.parse_args()

    mapping = None
    if args.mapping is not None:
        mapping = Mapping()
        mapping.init(args.mapping[0], args.mapping[1])

    compile_commands_path =get_valid_compile_commands_path(args.compile_commands)
    if compile_commands_path is None:
        exit(1)
    print (f"Found compile commands at: {compile_commands_path}")
        
    parsed_commands = CompileCommand.parse_compile_commands(compile_commands_path, mapping)
    for command in parsed_commands:
        # print (command.get_include_directories())
        file = SourceFile(command.file, command.get_include_directories())
        
        for chain in file.get_include_chains():
            print(f"Command {command.id}: {chain}")




if __name__ == '__main__':
    main()
