#pragma once
#include "../dependance/Opcode.h"
#include <string>
#include <sstream>
struct Instruction 
{
    Opcode opcode;
    int index;
    int offset;

    long double numValue;
    std::string stringValue;
    bool boolValue;
    

    Instruction(enum Opcode opcode, long double numValue) : opcode(opcode), numValue(numValue) {}
    Instruction(enum Opcode opcode, std::string stringValue) : opcode(opcode), stringValue(stringValue) {}
    Instruction(enum Opcode opcode, bool boolValue) : opcode(opcode), boolValue(boolValue) {}
    Instruction(enum Opcode opcode, int index) : opcode(opcode), index(index) {}
    Instruction(enum Opcode opcode, int index, int offset) : opcode(opcode), index(index), offset(offset) {}
    Instruction(enum Opcode opcode) : opcode(opcode) {}
};

