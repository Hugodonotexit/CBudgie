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

public:
    AST* tree;
    Translator(AST* tree);
    std::vector<std::string> run();
    
};
