#pragma once
#include "Translator.h"
#include "../VM/VM.h"
#include <filesystem>
class Interpreter : public Translator
{
private:
    /* data */
public:
    Interpreter(std::filesystem::path file);
};