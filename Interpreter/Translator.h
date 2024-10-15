#pragma once
#include <vector>
#include <filesystem>
#include <iostream>
#include <string>
#include <fstream>
#include "../dependance/AST.h"
class Translator
{
private:
    void writeToFile(std::vector<std::string>& bytecode, const std::filesystem::path& outputPath);
    void translate(std::vector<std::string>& bytecode, AST* tree);
public:
    Translator(AST* tree, std::filesystem::path destination);
};
