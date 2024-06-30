#include "parser.h"
Parser_var::~Parser_var() {}
Parser_var::Parser_var(vector<vector<Token>> &token) : tokens(token) {}

void Parser_var::coutConfig(int &line, int &pos) {
  coutConfig(line, pos, true);
}
void Parser_var::coutConfig(int &line, int &pos, bool isCIN) {
  int _i = line;
  int _j = pos + 1;
  if (_j >= tokens[_i].size() || tokens[_i][_j].type != TokenType::L_RBACKET) {
    error(ERROR::BRACKET, _i, _j);
  }
  stringstream ss;
  bool foundClosingBracket = false;
  while (!foundClosingBracket) {
    if (_j >= tokens[_i].size()) {
      _i++;
      _j = 0;
    }
    if (_i >= tokens.size()) {
      error(ERROR::BRACKET, _i - 1, tokens[_i - 1].size());
    }

    switch (tokens[_i][_j].type) {
      case TokenType::TRUESTRING:
      case TokenType::NUMBER:
        ss << tokens[_i][_j].value;
        break;
      case TokenType::PLUS:
        _j++;
        continue;
        break;
      case TokenType::VARIABLISED_NUM: {
        shared_ptr<VariableType<long double>> var =
            dynamic_pointer_cast<VariableType<long double>>(
                variable[stoi(tokens[_i][_j].value)]);
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
                variable[stoi(tokens[_i][_j].value)]);
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
                variable[stoi(tokens[_i][_j].value)]);
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
        error(ERROR::UNDEF, _i, _j);
        break;
    }
    _j++;
  }
  Consule::output(ss.str());
  if (isCIN) {
    cout << endl;
  }
  line = _i;
  pos = _j;
  return;
}
