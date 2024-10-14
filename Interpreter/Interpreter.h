#pragma once
#include "Parser.h"
#include "Translator.h"
#include "../VirtualMachine/VirtualMachine.h"
#include <filesystem>
class Interpreter
{
private:
    /* data */
public:
    Interpreter(std::filesystem::path file);
};