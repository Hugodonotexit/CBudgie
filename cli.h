#pragma once
#include "Lexer.h"
#include "Parser.h"
#include "Translator.h"
#include "VirtualMachine.h"
#include <string>
#include <exception>
#include <iostream>
#include <vector>
#include <sstream>
class cli
{
private:
    Lexer Lex;
    Parser Pars;
    Translator Trans;
    VirtualMachine VM;
    int argc;
    std::vector<std::string> argv;
    void runline();
    void run_argv();
    bool isExpected(std::string expected, std::string actual);
public:
    cli(Lexer Lexer, Parser Parser, Translator Translator, VirtualMachine VirtualMachine, int argc, char* argv[]);
    ~cli();
};

