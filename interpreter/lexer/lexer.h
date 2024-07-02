#ifndef LEXER_H
#define LEXER_H

#include <cctype>
#include <cmath>
#include <iostream>
#include <map>
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "var.h"

using namespace std;

class Lexer {
 private:
  map<int, int> range;
  vector<Token> tokens;
  vector<map<string, TokenType>> tokenPatterns;
  inline bool isChar(char a) {
    if (isalpha(a) || isdigit(a) || a == '_') {
      return true;
    }
    return false;
  }

 protected:
  vector<string> input;
  inline ~Lexer() {}
  inline Lexer() {
    tokenPatterns = {{{"^", TokenType::POW},
                      {"+", TokenType::PLUS},
                      {"-", TokenType::MINUS},
                      {"*", TokenType::TIMES},
                      {"/", TokenType::DIVIDE},
                      {"%", TokenType::MODE},
                      {"=", TokenType::EQUAL},
                      {">", TokenType::GREATER},
                      {"<", TokenType::SMALLER},
                      {"&", TokenType::AND},
                      {"|", TokenType::OR},
                      {"!", TokenType::NOT},
                      //{"@", TokenType::ADDRESS},
                      {",", TokenType::COMMA},
                      {":", TokenType::COLON},
                      {"(", TokenType::L_RBACKET},
                      {")", TokenType::R_RBACKET},
                      {"[", TokenType::L_SQBACKET},
                      {"]", TokenType::R_SQBACKET},
                      {"{", TokenType::L_SBACKET},
                      {"}", TokenType::R_SBACKET}},
                     {
                         {"ln", TokenType::LN},
                         {"if", TokenType::IF},
                         //{"++", TokenType::INC},
                         //{"--", TokenType::DEC},
                         {"!=", TokenType::NOT_EQUIVALENT},
                         {"==", TokenType::EQUIVALENT},
                         {"!|", TokenType::XOR},
                         {">=", TokenType::E_GREATER},
                         {"<=", TokenType::E_SMALLER},
                     },
                     {{"def", TokenType::DEF},
                      {"let", TokenType::LET},
                      {"cin", TokenType::CIN},
                      {"for", TokenType::FOR},
                      {"sin", TokenType::SIN},
                      {"cos", TokenType::COS},
                      {"tan", TokenType::TAN},
                      {"abs", TokenType::ABS}},
                     {//{"case", TokenType::CASE},
                      {"else", TokenType::ELSE},
                      {"asin", TokenType::ASIN},
                      {"acos", TokenType::ACOS},
                      {"atan", TokenType::ATAN},
                      {"sinh", TokenType::SINH},
                      {"cosh", TokenType::COSH},
                      {"tanh", TokenType::TANH},
                      {"True", TokenType::TRUE},
                      {"cout", TokenType::COUT}},
                     {{"False", TokenType::FALSE},
                      {"asinh", TokenType::ASINH},
                      {"acosh", TokenType::ACOSH},
                      {"atanh", TokenType::ATANH},
                      {"while", TokenType::WHILE},
                      {"break", TokenType::BREAK}},
                     {//{"switch", TokenType::SWITCH},
                      {"return", TokenType::RETURN}},
                     {
                         //{"default", TokenType::DEFAULT},
                     },
                     {{"continue", TokenType::CONTINUE}}};
  }

 public:
  inline vector<Token>& tokenize() {
    for (int line = 0; line < input.size(); line++) {
      int pos = 0;
      while (pos < input[line].size()) {
        if (isspace(input[line][pos])) {
          pos++;
          continue;
        } else if (input[line][pos] == '#') {
          break;
        }

        bool isFound = false;
        for (int i = 0; i < tokenPatterns.size(); i++) {
          auto it = tokenPatterns[i].find(input[line].substr(pos, i + 1));
          if (it != tokenPatterns[i].end()) {
            isFound = true;
            tokens.emplace_back(it->second, it->first);
            pos += (i + 1);
            break;
          }
        }
        if (isFound) continue;

        if (input[line][pos] == '\"') {
          bool isFound = false;
          int i = pos;
          do {
            i++;
            if (input[line][i] == '\"') {
              isFound = true;
            } else if (i == input[line].size()) {
              throw runtime_error("Missing a closing \" at" + to_string(line) +
                                  ":" + to_string(pos));
            }

          } while (!isFound);
          string str = input[line].substr(pos + 1, i - pos - 1);
          int x = 0;
          tokens.emplace_back(TokenType::TRUESTRING, str);
          pos = i + 1;
          continue;
        }

        if (isdigit(input[line][pos]) || input[line][pos] == '.') {
          int dot = 0;
          if (input[line][pos] == '.') {
            dot++;
          }

          bool isNum = true;
          int start = pos;
          int _pos_ = pos + 1;

          while (isNum) {
            if (isdigit(input[line][_pos_]) || input[line][_pos_] == '.') {
              if (input[line][pos] == '.') {
                dot++;
              }
              if (dot > 1) {
                throw runtime_error("Multiple Dot at" + to_string(line) + ":" +
                                    to_string(_pos_));
              }
              _pos_++;
              if (_pos_ == input[line].size()) {
                break;
              }
              continue;
            } else {
              isNum = false;
            }
          }
          int length = _pos_ - start;
          tokens.emplace_back(TokenType::NUMBER,
                              input[line].substr(start, length));
          pos += length;
          continue;
        }

        int i = pos;
        do {
          i++;
          if (i == input[line].size()) {
            break;
          }
        } while (isChar(input[line][i]));
        tokens.emplace_back(TokenType::STRING,
                            input[line].substr(pos, i - pos));
        pos = i;
      }
      range[tokens.size()] = line + 1;
    }
    return tokens;
  }
  inline map<int, int>& getrange() { return range; }
};

#endif
