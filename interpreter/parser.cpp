#include "parser.h"
Parser::Parser(vector<vector<Token>> &token) : tokens(token) {
  for (int line = 0; line < tokens.size(); line++) {
#pragma omp parallel for
    for (int pos = 0; pos < tokens[line].size(); pos++) {
      switch (tokens[line][pos].type) {
        case TokenType::FILE:
          defFile(line, pos);
          break;
        case TokenType::DEF:
          defScope<Function>(line, pos);
          break;
          /*
                  case TokenType::FOR:
                    defScope<For>(line, pos);
                    break;
                  case TokenType::WHILE:
                    defScope<While>(line, pos);
                    break;
                  case TokenType::IF:
                    defScope<If>(line, pos);
                    break;
                  case TokenType::SWITCH:
                    defScope<Switch>(line, pos);
                    break;
                  case TokenType::LET:
                    defVar(line, pos);
                    break;
          */
      }
    }
  }
  runFunction(mainIndex);
}

void Parser::runFunction(int index) {
  const Scope::Location &startPos = scope[index]->getStartPos();
  const Scope::Location &endPos = scope[index]->getEndPos();
  for (int i = startPos.line; i <= endPos.line; i++) {
    for (int j = 0; j < tokens[i].size(); j++) {
      if (i == startPos.line && j < startPos.pos) {
        j = startPos.pos;
      } else if (i == endPos.line && j > endPos.pos) {
        break;
      }
      switch (tokens[i][j].type) {
        case TokenType::LET:
          defVar(i, j, endPos.line, endPos.pos);
          break;
        case TokenType::CIN:
          if (tokens[i][j - 1].type == TokenType::EQUAL) {
            if (tokens[i][j - 2].type == TokenType::VARIABLISED_STR) {
              std::shared_ptr<VariableType<string>> var = dynamic_pointer_cast<VariableType<string>>(variable[stoi(tokens[i][j - 2].value)]);
              coutConfig(i, j);
              string str = Consule::input();
              var->newvalue(str);
            }
          } else {
            coutConfig(i, j);
            Consule::input();
          }
          break;
        case TokenType::COUT:
          coutConfig(i, j);
          break;
      }
    }
  }
  return;
}

void Parser::coutConfig(int &line, int &pos) {
  int _i = line;
  int _j = pos + 1;
  if (_j >= tokens[_i].size() || tokens[_i][_j].type != TokenType::L_RBACKET) {
    throw runtime_error("Missing bracket at " + to_string(_i) + ":" +
                        to_string(_j));
  }
  stringstream ss;
  bool foundClosingBracket = false;
  while (!foundClosingBracket) {
    if (_j >= tokens[_i].size()) {
      _i++;
      _j = 0;
    }
    if (_i >= tokens.size()) {
      throw runtime_error("Missing bracket at " + to_string(_i) + ":" +
                          to_string(_j - 1));
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
        std::shared_ptr<VariableType<long double>> var =
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
        std::shared_ptr<VariableType<string>> var =
            dynamic_pointer_cast<VariableType<string>>(
                variable[stoi(tokens[_i][_j].value)]);
        for (int l = 0; l < var->getSize(); l++) {

          if (l > 0) {
            ss << " ";
          }
          ss << var->getvalue(l);
        }
      } break;
      case TokenType::R_RBACKET:
        foundClosingBracket = true;
        break;
    }
    _j++;
  }
  Consule::output(ss.str());
  line = _i;
  pos = _j;
  return;
}

void Parser::defVar(int line, int pos, int end_line, int end_pos) {
  shared_ptr<Variable> var;
  TokenType type;
  int step = 0;
  size_t size = 1;
  if (tokens[line][pos + 2].type == TokenType::L_SQBACKET) {
    if (tokens[line][pos + 4].type == TokenType::R_SQBACKET) {
      step = 3;
      size = stoi(tokens[line][pos + 3].value);
    } else {
      throw runtime_error("Missing close bracket at " + to_string(line) + ":" +
                          to_string(pos + 4));
    }
  }

  if (tokens[line][pos + 2 + step].type == TokenType::EQUAL) {
    int step2 = 0;
    if (tokens[line][pos + 3 + step].type == TokenType::L_SQBACKET) {
      step2++;
    }
    switch (tokens[line][pos + 3 + step + step2].type) {
      case TokenType::CIN: {
          type = TokenType::VARIABLISED_STR;
          vector<string *> array;
          for (int i = 0; i < size; i++) {
          int _i = i + pos + 3 + step + step2;
          coutConfig(line, _i);
          i = _i - (pos + 3 + step + step2);
          string str = Consule::input();
          array.push_back(new string(str));
          if (pos + 3 + step + step2 + i >= tokens[line].size() || tokens[line][pos + 3 + step + step2 + i].type == TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(new (string)(*array[array.size() - 1]));
        }
        var = make_shared<VariableType<string>>(tokens[line][pos + 1].value, size, array);
      }
          break;
      case TokenType::TRUESTRING: {
        type = TokenType::VARIABLISED_STR;
        vector<string *> array;
        for (int i = 0; i < size; i++) {
          array.push_back(
              new string(tokens[line][pos + 3 + step + step2 + i].value));
          if (pos + 3 + step + step2 + i >= tokens[line].size() ||
              tokens[line][pos + 3 + step + step2 + i].type ==
                  TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(new (string)(*array[array.size() - 1]));
        }
        var = make_shared<VariableType<string>>(tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::NUMBER: {
        type = TokenType::VARIABLISED_NUM;
        long double num = stold(tokens[line][pos + 3 + step + step2].value);
        vector<long double *> array;
        for (int i = 0; i < size; i++) {
          array.push_back(new (long double)(
              stold(tokens[line][pos + 3 + step + step2 + i].value)));
          if (pos + 3 + step + step2 + i >= tokens[line].size() ||
              tokens[line][pos + 3 + step + step2 + i].type ==
                  TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(new (long double)(*array[array.size() - 1]));
        }
        var = make_shared<VariableType<long double>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::FALSE:
      case TokenType::TRUE: {
        vector<bool *> array;
        for (int i = 0; i < size; i++) {
          switch (tokens[line][pos + 3 + step + step2 + i].type) {
            case TokenType::TRUE:
              array.push_back(new bool(true));
              break;
            case TokenType::FALSE:
              array.push_back(new bool(false));
              break;
          }
          if (pos + 3 + step + step2 + i >= tokens[line].size() ||
              tokens[line][pos + 3 + step + step2 + i].type ==
                  TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(new bool(array[array.size() - 1]));
        }
        var = make_shared<VariableType<bool>>(tokens[line][pos + 1].value, size,
                                              array);
      } break;
      case TokenType::FUNCTIONISED:
        runFunction(stoi(tokens[line][pos + 3 + step + step2].value));
        defVar(line, pos, end_line, end_pos);
        break;
    }
  } else {
    throw runtime_error("Varaible not initialised at " + to_string(line) + ":" +
                        to_string(pos));
    abort();
  }
#pragma omp parallel for
  for (int i = line; i <= end_line; i++) {
#pragma omp parallel for
    for (int j = 0; j < tokens[i].size(); j++) {
      if (i == line && j < pos) {
        j = pos;
      } else if (i >= end_line && j >= end_pos) {
        break;
      }

      if (tokens[i][j].type == TokenType::STRING) {
        if (j > 0) {
          if (tokens[i][j - 1].type != TokenType::LET &&
              var->getname() == tokens[i][j].value) {
            tokens[i][j].type = type;
            tokens[i][j].value = to_string(variable.size());
          }
        } else {
          if (var->getname() == tokens[i][j].value) {
            tokens[i][j].type = type;
            tokens[i][j].value = to_string(variable.size());
          }
        }
      }
    }
  }
  variable.push_back(var);
  
  
}

void Parser::defFile(int line, int pos) {
  File file(tokens[line][pos + 1].value);
  int i = line;
  int j = pos;
  do {
    j++;
    if (j == tokens[i].size()) {
      i++;
      j = 0;
      if (i == tokens.size()) {
        i--;
        j = tokens[i].size() - 1;
        break;
      }
    }
  } while (!(tokens[i][j].value == "`"));
  file.setEndtPos(i, j);
  scope.push_back(make_shared<File>(file));
}

template <typename T>
void Parser::defScope(int line, int pos) {
  int openbracket = 0;
  int closebracket = 0;
  int openRoundBracket = 0;
  int closeRoundBracket = 0;
  int i = line;
  int j = pos;

  std::optional<T> scoOpt;

  if constexpr (std::is_same_v<T, Function>) {
    scoOpt.emplace(tokens[line][pos + 1].value);
  } else {
    scoOpt.emplace();
  }

  T &sco = scoOpt.value();

  while (openbracket != closebracket || openbracket == 0 || closebracket == 0) {
    j++;
    if (j >= tokens[i].size()) {
      i++;
      j = 0;
      if (i >= tokens.size()) {
        throw std::runtime_error("Missing close bracket at " + to_string(i) +
                                 ":" + to_string(j));
        abort();
      }
    }
    switch (tokens[i][j].type) {
      case TokenType::L_RBACKET:
        openRoundBracket++;
        break;
      case TokenType::R_RBACKET:
        closeRoundBracket++;
        break;
      case TokenType::L_SBACKET:
        openbracket++;
        break;
      case TokenType::R_SBACKET:
        closebracket++;
        break;
    }
    if (openRoundBracket == 1) {
      sco.setConStartPos(i, j);
      openRoundBracket++;
    } else if (closeRoundBracket == 1) {
      sco.setConEndPos(i, j);
      closeRoundBracket++;
    } else if (openbracket == 1) {
      sco.setStartPos(i, j);
      openbracket++;
      closebracket++;
    }
  }
  sco.setEndtPos(i, j);
  if constexpr (std::is_same_v<T, Function>) {
#pragma omp parallel for
    for (int i = 0; i < tokens.size(); i++) {
#pragma omp parallel for
      for (int j = 0; j < tokens[i].size(); j++) {
        if (tokens[i][j].type == TokenType::STRING) {
          if (j > 0) {
            if (tokens[i][j - 1].type != TokenType::DEF &&
                sco.getname() == tokens[i][j].value) {
              tokens[i][j].type = TokenType::FUNCTIONISED;
              tokens[i][j].value = to_string(scope.size());
            }
          } else {
            if (sco.getname() == tokens[i][j].value) {
              tokens[i][j].type = TokenType::FUNCTIONISED;
              tokens[i][j].value = to_string(scope.size());
            }
          }
        }
      }
    }

    if (sco.getname() == "main") {
      mainIndex = scope.size();
    }
  }
  scope.push_back(std::make_shared<T>(sco));
};

Parser::~Parser() {}