#pragma once
#include "Parser.h"
#include "Translator.h"
#include "Lexer.h"
#include "../VirtualMachine/VirtualMachine.h"
#include <filesystem>
class Interpreter
{
private:
    void run(std::filesystem::path file, std::filesystem::path path, bool translate_only);
    std::vector<std::string> openfile(const std::filesystem::path& filePath);
    void writeToFile(std::vector<std::string>& bytecode, const std::filesystem::path& outputPath);
public:
    Interpreter(std::filesystem::path file, bool translate_only = false);
    Interpreter(std::filesystem::path file, std::filesystem::path path);
};