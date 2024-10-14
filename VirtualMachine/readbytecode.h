#pragma once
#include <fstream>
#include <filesystem>
#include <string>
#include <iostream>
#include "Instruction.h"
#include <array>
#include <vector>
#include <algorithm>
#include "../dependance/Opcode.h"
class readbytecode
{
private:
    Instruction read_line(const std::string &line);
    Opcode parseOpcode(const std::string& str);
    std::string parseStringOperand(std::istringstream& stream);
    long double parseNumOperand(std::istringstream& stream);
    std::array<int, 3> parseIntOperands(std::istringstream& stream);
public:
    std::vector<Instruction> read_file(const std::filesystem::path & filename);
};
