#ifndef LEXER_H
#define LEXER_H

#include "var.h"
#include <iostream>
#include <vector>
#include <string>
#include <regex>
#include <stdexcept>
#include <cctype>

using namespace std;

class Lexer
{
private:
    vector<string> input;
    vector<pair<TokenType, string>> tokenPatterns;
    bool isChar(char a);
public:
    Lexer(const vector<string>& input);
    vector<vector<Token>> tokenize();
    ~Lexer();
};

#endif

