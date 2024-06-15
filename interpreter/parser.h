#ifndef PARSER_H
#define PARSER_H
#include <cstdint>
#include <vector>
#include <memory>
#include <algorithm>
#include <type_traits>
#include <optional>

#include "lexer.h"
#include "ast.h"

class Parser
{
private:

    vector<vector<Token>> *tokens;
    vector<int_least8_t> INT8;
    vector<int_least16_t> INT16;
    vector<int_least32_t> INT32;
    vector<int_least64_t> INT64;
    vector<uint_least8_t> UINT8;
    vector<uint_least16_t> UINT16;
    vector<uint_least32_t> UINT32;
    vector<uint_least64_t> UINT64;
    vector<float> FLOAT4;
    vector<double> FLOAT8;
    vector<long double> FLOAT16;
    vector<char> CHAR;
    vector<std::string> STRING;
    vector<wchar_t> WCHAR;
    vector<bool> BOOL;
    vector<std::shared_ptr<Scope>> scope;
public:
    Parser(vector<vector<Token>>& tokens);
    template<typename T>
    void defScope(int &line, int &pos);
    ~Parser();
};


#endif