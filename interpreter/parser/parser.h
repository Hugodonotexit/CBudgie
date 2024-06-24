#ifndef PARSER_H
#define PARSER_H
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <optional>
#include <future>
#include <omp.h>
#include <sstream>
#include <iostream>
#include "../depen/ast.h"
#include "function.hpp"

class Parser
{
private:
    vector<vector<Token>> tokens;

    vector<shared_ptr<Scope>> scope;
    vector<shared_ptr<Variable>> variable;
    int mainIndex;
    template<typename T>
    void defScope(int line, int pos);
    void defFile(int line, int pos);
    void defVar(int& line, int& pos, int end_line, int end_pos);

    void runFunction(int index);
    void runFunction(int index, int __line__, int __pos__);
    
    void coutConfig(int &line, int &pos);
    template<typename T>
    T doMath(int line, int pos, int end_line, int end_pos);
public:
    Parser(vector<vector<Token>> &tokens);
    ~Parser();
};


#endif