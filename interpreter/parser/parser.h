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

class Parser_error
{
protected:
    Parser_error();
    ~Parser_error();
    void error(ERROR type,int line, int pos);
};



class Parser_var : public Parser_error
{
protected:
    vector<vector<Token>> tokens;
    vector<shared_ptr<Scope>> scope;
    vector<shared_ptr<Variable>> variable;
    int mainIndex;;

    Parser_var(vector<vector<Token>> &tokens);
    ~Parser_var();

    void coutConfig(int &line, int &pos);
    void coutConfig(int &line, int &pos, bool isCIN);
};

class BoolSupport : public Parser_var
{
private:
    void cleanBracket();
    void cleanNAN();
    void locateBrackets();
    void locateOps();
    
protected:
    vector<Token> tokens_copy;
    vector<int> operators[2];
    vector<std::pair<int, int>> brackets;
    BoolSupport(vector<vector<Token>> &tokens);
    ~BoolSupport();

    void scanFunc();
    void calculate(int index);
};

class Parser_main : public BoolSupport
{
private:
    template<typename T>
    void defScope(int line, int pos);
    void defFile(int line, int pos);
    void defVar(int& line, int& pos, int end_line, int end_pos);
    void defVar(int& line, int& pos, int end_line, int end_pos, vector<shared_ptr<Variable>> pass);

    void runFunction(int index);
    void runFunction(int index, int __line__, int __pos__);
    void runFunction(int index, vector<shared_ptr<Variable>> var);
    void runFunction(int index, int __line__, int __pos__, vector<shared_ptr<Variable>> var);
    
    template<typename T>
    T doMath(int& line, int& pos, int end_line, int end_pos);
    bool boolOP(int& line, int& pos, int end_line, int end_pos);
public:
    Parser_main(vector<vector<Token>> &tokens);
    ~Parser_main();
};


#endif