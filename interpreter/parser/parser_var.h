#ifndef PARSER_VAR_H
#define PARSER_VAR_H
#include <omp.h>
#include <memory>
#include <sstream>
#include <vector>

#include "../depen/ast.h"
#include "parser_error.h"

class Parser_var : public Parser_error {
 protected:
  vector<shared_ptr<Scope>> scope;
  vector<shared_ptr<Variable>> variable;
  int mainIndex;

  ~Parser_var() {}
  Parser_var(vector<Token> &token, map<int,int> &range) : Parser_error(token, range) {}

  inline void coutConfig(int &pos) { coutConfig(pos, true); }
  inline void coutConfig(int &pos, bool isCIN) {
    int _j = pos + 1;
    if (_j >= tokens.size() ||
        tokens[_j].type != TokenType::L_RBACKET) {
      error(ERROR::BRACKET, _j);
    }
    stringstream ss;
    bool foundClosingBracket = false;
    while (!foundClosingBracket) {
      if (_j >= tokens.size()) {
        error(ERROR::BRACKET, tokens.size());
      }
      switch (tokens[_j].type) {
        case TokenType::TRUESTRING:
        case TokenType::NUMBER:
          ss << tokens[_j].value;
          break;
        case TokenType::PLUS:
          _j++;
          continue;
          break;
        case TokenType::VARIABLISED_NUM: {
          shared_ptr<VariableType<long double>> var =
              dynamic_pointer_cast<VariableType<long double>>(
                  variable[stoi(tokens[_j].value)]);
          for (int l = 0; l < var->getSize(); l++) {
            if (l > 0) {
              ss << " ";
            }
            ss << var->getvalue(l);
          }
        } break;
        case TokenType::VARIABLISED_STR: {
          shared_ptr<VariableType<string>> var =
              dynamic_pointer_cast<VariableType<string>>(
                  variable[stoi(tokens[_j].value)]);
          for (int l = 0; l < var->getSize(); l++) {
            if (l > 0) {
              ss << " ";
            }
            ss << var->getvalue(l);
          }
        } break;
        case TokenType::TRUE:
          ss << "True";
          break;
        case TokenType::FALSE:
          ss << "False";
          break;
        case TokenType::VARIABLISED_BOOL: {
          shared_ptr<VariableType<bool>> var =
              dynamic_pointer_cast<VariableType<bool>>(
                  variable[stoi(tokens[_j].value)]);
          for (int l = 0; l < var->getSize(); l++) {
            if (l > 0) {
              ss << " ";
            }
            if (var->get_value(l)) {
              ss << "True";
            } else {
              ss << "False";
            }
          }
        } break;
        case TokenType::R_RBACKET:
          foundClosingBracket = true;
          break;
        case TokenType::STRING:
          error(ERROR::UNDEF, _j);
          break;
      }
      _j++;
    }
    Consule::output(ss.str());
    if (isCIN) {
      cout << endl;
    }
    pos = _j;
    return;
  }
};
#endif