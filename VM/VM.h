#pragma once
#include <stack>
#include <vector>
#include "readbytecode.h"
#include "Instruction.h"
#include <variant>
#include <filesystem>
#include <cmath>
#include "WrappedVar.h"

class VM
{
private:
    std::stack<std::stack<WrappedVar>> stack;

    std::vector<std::vector<WrappedVar>> var;

    std::vector<Instruction> instructions;

    std::stack<int> functionPointers;
public:
    VM(const std::filesystem::path &filePath);
};