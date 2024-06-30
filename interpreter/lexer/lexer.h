#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <iostream>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include <cmath>

#include "var.h"

using namespace std;

class Lexer {
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
