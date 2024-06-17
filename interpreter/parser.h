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
#include "lexer.h"
#include "ast.h"

class Parser
{
private:
    vector<vector<Token>> tokens;

    vector<std::shared_ptr<Scope>> scope;

    template<typename T>
    void defScope(int &line, int &pos);
    void defFile(int &line, int &pos);
    std::shared_ptr<Variable> defVar(int &line, int &pos);
public:
    Parser(vector<vector<Token>> &tokens);
    ~Parser();
};


#endif