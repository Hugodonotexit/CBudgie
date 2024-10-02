#pragma once
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stack>
#include <utility>
#include "Lexer.h"
#include "../dependance/Token.h"
#include "../dependance/Opcode.h"
#include <deque>
class Translator
{
private:
    std::filesystem::path filePath;
    
    void translate(std::vector<std::vector<Token>>& tokenized_code, std::vector<std::string>& bytecode);

    void writeToFile(std::vector<std::string>& bytecode, const std::filesystem::path& outputPath);
public:
    Translator(std::filesystem::path file);
    Translator(std::filesystem::path file, std::filesystem::path destination);
};