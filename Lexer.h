#pragma once
#include "utility.h"
#include <vector>
#include <string>
class Lexer
{
private:
    /* data */
public:
    Lexer(/* args */);
    std::vector<Token> run(std::vector<std::string> cmd);
    ~Lexer();
};


