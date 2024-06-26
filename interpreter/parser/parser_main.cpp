#include "parser.h"
Parser_main::~Parser_main() {}
Parser_main::Parser_main(vector<vector<Token>> &token) : Parser_var(token) {
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
      }
    }
  }
  runFunction(mainIndex);
}

void Parser_main::runFunction(int index) { runFunction(index, -1, -1); }

void Parser_main::runFunction(int index, int _line_, int _pos_) {
  vector<shared_ptr<Variable>> var;
  runFunction(index, -1, -1, var);
}
void Parser_main::runFunction(int index, vector<shared_ptr<Variable>> var) {
  runFunction(index, -1, -1, var);
}
void Parser_main::runFunction(int index, int _line_, int _pos_,
                              vector<shared_ptr<Variable>> var) {
  int varStartSize = variable.size();
  const Scope::Location &startPos = scope[index]->getStartPos();
  const Scope::Location &endPos = scope[index]->getEndPos();
  Scope::Location conStartPos = scope[index]->getConStartPos();
  Scope::Location conEndPos = scope[index]->getConEndPos();
  if (conEndPos.pos - conStartPos.pos != 1) {
    for (int i = conStartPos.pos + 1; i < tokens[conStartPos.line].size();
         i++) {
      if (tokens[conStartPos.line][i].type == TokenType::LET) {
        defVar(conStartPos.line, i, endPos.line, endPos.pos, var);
        var.erase(var.begin());
      } else if (tokens[conStartPos.line][i].type == TokenType::R_RBACKET) {
        break;
      }
    }
  }

  for (int i = startPos.line; i <= endPos.line; i++) {
#pragma omp parallel for
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
              shared_ptr<VariableType<string>> var =
                  dynamic_pointer_cast<VariableType<string>>(
                      variable[stoi(tokens[i][j - 2].value)]);
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
        case TokenType::FUNCTIONISED:
          if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
            if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[i][j].value), i, j);
            } else {
              int _j = 1;
              vector<shared_ptr<Variable>> passingVars;
              while (tokens[i][j + _j].type != TokenType::R_RBACKET) {
                switch (tokens[i][j + _j].type) {
                  case TokenType::BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[i][j + _j].value)]));
                    passingVars.push_back(make_shared<VariableType<bool>>(var));
                  } break;
                  case TokenType::TRUE: {
                    VariableType<bool> var;
                    var.newvalue(true);
                    passingVars.push_back(make_shared<VariableType<bool>>(var));
                  } break;
                  case TokenType::FALSE: {
                    VariableType<bool> var;
                    var.newvalue(false);
                    passingVars.push_back(make_shared<VariableType<bool>>(var));
                  } break;
                  case TokenType::NUMBER:
                  case TokenType::VARIABLISED_NUM: {
                    VariableType<long double> var;
                    var.newvalue(doMath<long double>(i, j + _j, i, j + _j));
                    passingVars.push_back(
                        make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.newvalue(doMath<string>(i, j + _j, i, j + _j));
                    passingVars.push_back(
                        make_shared<VariableType<string>>(var));
                  } break;
                }
                _j++;
              }
              runFunction(stoi(tokens[i][j].value), i, j, passingVars);
            }
            j--;
          } else {
            error(ERROR::BRACKET, i, j + 1);
          }
        case TokenType::RETURN:
          switch (tokens[i][j + 1].type) {
            case TokenType::TRUE:
              tokens[_line_][_pos_].type = TokenType::TRUE;
              return;
            case TokenType::FALSE:
              tokens[_line_][_pos_].type = TokenType::FALSE;
              return;
            case TokenType::NUMBER:
            case TokenType::VARIABLISED_NUM:
              tokens[_line_][_pos_].type = TokenType::NUMBER;
              tokens[_line_][_pos_].value = to_string(
                  doMath<long double>(i, j + 1, i, tokens[i].size() - 1));
              return;
            case TokenType::TRUESTRING:
            case TokenType::VARIABLISED_STR:
              tokens[_line_][_pos_].type = TokenType::TRUESTRING;
              tokens[_line_][_pos_].value =
                  doMath<string>(i, j + 1, i, tokens[i].size() - 1);
              return;
            case TokenType::FUNCTIONISED:
              if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
                if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
                  runFunction(stoi(tokens[i][j].value), i, j);
                } else {
                  int _j = 1;
                  vector<shared_ptr<Variable>> vars;
                  while (tokens[i][j + _j].type != TokenType::R_RBACKET) {
                    switch (tokens[i][j + _j].type) {
                      case TokenType::BOOL: {
                        VariableType<bool> var;
                        var = *(dynamic_pointer_cast<VariableType<bool>>(
                            variable[stoi(tokens[i][j + _j].value)]));
                        vars.push_back(make_shared<VariableType<bool>>(var));
                      }
                      case TokenType::TRUE: {
                        VariableType<bool> var;
                        var.newvalue(true);
                        vars.push_back(make_shared<VariableType<bool>>(var));
                      } break;
                      case TokenType::FALSE: {
                        VariableType<bool> var;
                        var.newvalue(false);
                        vars.push_back(make_shared<VariableType<bool>>(var));
                      } break;
                      case TokenType::NUMBER:
                      case TokenType::VARIABLISED_NUM: {
                        VariableType<long double> var;
                        var.newvalue(doMath<long double>(i, j + _j, i, j + _j));
                        vars.push_back(
                            make_shared<VariableType<long double>>(var));
                      } break;
                      case TokenType::TRUESTRING:
                      case TokenType::VARIABLISED_STR: {
                        VariableType<string> var;
                        var.newvalue(doMath<string>(i, j + _j, i, j + _j));
                        vars.push_back(make_shared<VariableType<string>>(var));
                      } break;
                    }
                    _j++;
                  }
                  runFunction(stoi(tokens[i][j].value), i, j, vars);
                }
                j--;
              } else {
                error(ERROR::BRACKET, i, j + 1);
              }
              continue;
            case TokenType::STRING:
              error(ERROR::UNDEF, i, j);
              break;
          }
        case TokenType::EQUAL:
          switch (tokens[i][j - 1].type) {
            case TokenType::VARIABLISED_NUM: {
              shared_ptr<VariableType<long double>> var =
                  dynamic_pointer_cast<VariableType<long double>>(
                      variable[stoi(tokens[i][j - 1].value)]);
              var->newvalue(
                  doMath<long double>(i, j + 1, i, tokens[i].size() - 1));
            } break;
            case TokenType::VARIABLISED_STR: {
              shared_ptr<VariableType<string>> var =
                  dynamic_pointer_cast<VariableType<string>>(
                      variable[stoi(tokens[i][j - 1].value)]);
              var->newvalue(doMath<string>(i, j + 1, i, tokens[i].size() - 1));
            } break;
            case TokenType::VARIABLISED_BOOL: {
              shared_ptr<VariableType<bool>> var =
                  dynamic_pointer_cast<VariableType<bool>>(
                      variable[stoi(tokens[i][j - 1].value)]);
              if (tokens[i][j + 1].type == TokenType::TRUE) {
                var->newvalue(true);
              } else if (tokens[i][j + 1].type == TokenType::FALSE) {
                var->newvalue(false);
              }
            } break;
            case TokenType::STRING:
              error(ERROR::UNDEF, i, j);
              break;
          }
      }
    }
  }
  if (variable.size() > varStartSize) {
    variable.erase(variable.begin() + varStartSize, variable.end());
  }

  return;
}
void Parser_main::defVar(int &line, int &pos, int end_line, int end_pos) {
  vector<shared_ptr<Variable>> pass;
  defVar(line, pos, end_line, end_pos, pass);
}
void Parser_main::defVar(int &line, int &pos, int end_line, int end_pos,
                         vector<shared_ptr<Variable>> pass) {
  shared_ptr<Variable> var;
  TokenType type;
  size_t size = 1;
  int _pos;
  string name;
  if (pass.empty()) {
    _pos = pos++;
    name = tokens[line][pos].value;
    if (tokens[line][pos + 1].type == TokenType::L_SQBACKET) {
      if (tokens[line][pos + 3].type == TokenType::R_SQBACKET) {
        size = stoi(tokens[line][pos + 2].value);
        pos += 3;
      } else {
        error(ERROR::BRACKET, line, pos + 4);
      }
    }
    pos++;
  }

  if (tokens[line][pos].type == TokenType::EQUAL) {
    if (tokens[line][pos + 1].type == TokenType::L_SQBACKET) {
      pos++;
    }
    pos++;
    switch (tokens[line][pos].type) {
      case TokenType::STRING:
        error(ERROR::UNDEF, line, pos);
        break;
      case TokenType::TRUESTRING: {
        type = TokenType::VARIABLISED_STR;
        vector<string> array;
        for (int i = 0; i < size; i++) {
          array.push_back(tokens[line][pos + i].value);
          if (pos + i >= tokens[line].size() ||
              tokens[line][pos + i].type == TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        var = make_shared<VariableType<string>>(name, array);
      } break;
      case TokenType::NUMBER: {
        type = TokenType::VARIABLISED_NUM;
        vector<long double> array;
        for (int i = 0; i < size; i++) {
          array.push_back(stold(tokens[line][pos + i].value));
          if (pos + i >= tokens[line].size() ||
              tokens[line][pos + i].type == TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        var = make_shared<VariableType<long double>>(name, array);
      } break;
      case TokenType::VARIABLISED_NUM: {
        type = TokenType::VARIABLISED_NUM;
        shared_ptr<VariableType<long double>> _var =
            dynamic_pointer_cast<VariableType<long double>>(
                variable[stoi(tokens[line][pos].value)]);
        var = make_shared<VariableType<long double>>(name, _var->getvector());
      } break;
      case TokenType::VARIABLISED_STR: {
        type = TokenType::VARIABLISED_STR;
        shared_ptr<VariableType<string>> _var =
            dynamic_pointer_cast<VariableType<string>>(
                variable[stoi(tokens[line][pos].value)]);
        var = make_shared<VariableType<string>>(name, _var->getvector());
      } break;
      case TokenType::VARIABLISED_BOOL: {
        type = TokenType::VARIABLISED_BOOL;
        shared_ptr<VariableType<bool>> _var =
            dynamic_pointer_cast<VariableType<bool>>(
                variable[stoi(tokens[line][pos].value)]);
        var = make_shared<VariableType<bool>>(name, _var->getvector());
      } break;
      case TokenType::FALSE:
      case TokenType::TRUE: {
        type = TokenType::VARIABLISED_BOOL;
        vector<bool> array;
        for (int i = 0; i < size; i++) {
          switch (tokens[line][pos + i].type) {
            case TokenType::TRUE:
              array.push_back(true);
              break;
            case TokenType::FALSE:
              array.push_back(false);
              break;
            default:
              error(ERROR::OTHER, line, pos + i);
              break;
          }
          if (pos + i >= tokens[line].size() ||
              tokens[line][pos + i].type == TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        var = make_shared<VariableType<bool>>(name, array);
      } break;
      case TokenType::FUNCTIONISED:
        if (tokens[line][pos + 1].type == TokenType::L_RBACKET) {
          if (tokens[line][pos + 2].type == TokenType::R_RBACKET) {
            runFunction(stoi(tokens[line][pos].value), line, pos);
          } else {
            int _j = 1;
            vector<shared_ptr<Variable>> vars;
            while (tokens[line][pos + _j].type != TokenType::R_RBACKET) {
              switch (tokens[line][pos + _j].type) {
                case TokenType::BOOL: {
                  VariableType<bool> var;
                  var = *(dynamic_pointer_cast<VariableType<bool>>(
                      variable[stoi(tokens[line][pos + _j].value)]));
                  vars.push_back(make_shared<VariableType<bool>>(var));
                }
                case TokenType::TRUE: {
                  VariableType<bool> var;
                  var.newvalue(true);
                  vars.push_back(make_shared<VariableType<bool>>(var));
                } break;
                case TokenType::FALSE: {
                  VariableType<bool> var;
                  var.newvalue(false);
                  vars.push_back(make_shared<VariableType<bool>>(var));
                } break;
                case TokenType::NUMBER:
                case TokenType::VARIABLISED_NUM: {
                  VariableType<long double> var;
                  var.newvalue(
                      doMath<long double>(line, pos + _j, line, pos + _j));
                  vars.push_back(make_shared<VariableType<long double>>(var));
                } break;
                case TokenType::TRUESTRING:
                case TokenType::VARIABLISED_STR: {
                  VariableType<string> var;
                  var.newvalue(doMath<string>(line, pos + _j, line, pos + _j));
                  vars.push_back(make_shared<VariableType<string>>(var));
                } break;
              }
              _j++;
            }
            runFunction(stoi(tokens[line][pos].value), line, pos, vars);
          }
        } else {
          error(ERROR::BRACKET, line, pos + 1);
        }
        defVar(line, _pos, end_line, end_pos);
        break;
    }
  } else if (!pass.empty()) {
    if (tokens[line][pos].type == TokenType::LET) {
      if (shared_ptr<VariableType<long double>> num =
              dynamic_pointer_cast<VariableType<long double>>(pass[0])) {
        type = TokenType::VARIABLISED_NUM;
        vector<long double> arr = num->getvector();
        var = make_shared<VariableType<long double>>(
            tokens[line][pos + 1].value, arr);
      } else if (shared_ptr<VariableType<bool>> boo =
                     dynamic_pointer_cast<VariableType<bool>>(pass[0])) {
        type = TokenType::VARIABLISED_BOOL;
        vector<bool> arr = boo->getvector();
        var = make_shared<VariableType<bool>>(tokens[line][pos + 1].value, arr);
      } else if (shared_ptr<VariableType<string>> str =
                     dynamic_pointer_cast<VariableType<string>>(pass[0])) {
        type = TokenType::VARIABLISED_STR;
        vector<string> arr = str->getvector();
        var =
            make_shared<VariableType<string>>(tokens[line][pos + 1].value, arr);
      }
    }
  } else {
    error(ERROR::INIT_VAR, line, pos);
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

void Parser_main::defFile(int line, int pos) {
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
void Parser_main::defScope(int line, int pos) {
  int openbracket = 0;
  int closebracket = 0;
  int openRoundBracket = 0;
  int closeRoundBracket = 0;
  int i = line;
  int j = pos;

  optional<T> scoOpt;

  if constexpr (is_same_v<T, Function>) {
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
        error(ERROR::BRACKET, i, j);
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
  if constexpr (is_same_v<T, Function>) {
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
  scope.push_back(make_shared<T>(sco));
};

template <typename T>
T Parser_main::doMath(int line, int pos, int end_line, int end_pos) {
  T var;
  string str;
  for (int i = line; i <= end_line; i++) {
#pragma omp parallel for
    for (int j = 0; j < tokens[i].size(); j++) {
      T localVar;
      if (i == line && j < pos) {
        j = pos;
      } else if (i >= end_line && j > end_pos) {
        i++;
        j = 0;
        break;
      }
      switch (tokens[i][j].type) {
        case TokenType::FUNCTIONISED:
          if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
            if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[i][j].value), i, j);
            } else {
              int _j = 1;
              vector<shared_ptr<Variable>> vars;
              while (tokens[i][j + _j].type != TokenType::R_RBACKET) {
                switch (tokens[i][j + _j].type) {
                  case TokenType::BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[i][j + _j].value)]));
                    vars.push_back(make_shared<VariableType<bool>>(var));
                  }
                  case TokenType::TRUE: {
                    VariableType<bool> var;
                    var.pushValue(true);
                    vars.push_back(make_shared<VariableType<bool>>(var));
                  } break;
                  case TokenType::FALSE: {
                    VariableType<bool> var;
                    var.pushValue(false);
                    vars.push_back(make_shared<VariableType<bool>>(var));
                  } break;
                  case TokenType::NUMBER:
                  case TokenType::VARIABLISED_NUM: {
                    VariableType<long double> var;
                    var.pushValue(doMath<long double>(i, j + _j, i, j + _j));
                    vars.push_back(make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.pushValue(doMath<string>(i, j + _j, i, j + _j));
                    vars.push_back(make_shared<VariableType<string>>(var));
                  } break;
                }
                _j++;
              }
              runFunction(stoi(tokens[i][j].value), i, j, vars);
            }
            j--;
          } else {
            error(ERROR::BRACKET, i, j + 1);
          }
          continue;
        case TokenType::VARIABLISED_NUM: {
          tokens[i][j].type = TokenType::NUMBER;
          int index = 0;
          int _j = j;
          shared_ptr<VariableType<long double>> _var =
              dynamic_pointer_cast<VariableType<long double>>(
                  variable[stoi(tokens[i][j].value)]);
          if (_var->getSize() > 1) {
            _j++;
            if (tokens[i][j].type == TokenType::L_SQBACKET) {
              _j++;
              int startPos = _j;
              while (tokens[i][_j].type != TokenType::R_SQBACKET) {
                _j++;
                if (_j >= tokens[i].size()) {
                  error(ERROR::BRACKET, i, j);
                }
              }
              index = doMath<int>(i, startPos, i, _j);
            } else {
              error(ERROR::ARRAY_ADDING, i, j);
            }
          }
          tokens[i][j].value = to_string(_var->getvalue(index));
          j--;
        }
          continue;
        case TokenType::VARIABLISED_STR: {
          tokens[i][j].type = TokenType::TRUESTRING;
          int index = 0;
          int _j = j;
          shared_ptr<VariableType<string>> _var =
              dynamic_pointer_cast<VariableType<string>>(
                  variable[stoi(tokens[i][j].value)]);
          if (_var->getSize() > 1) {
            _j++;
            if (tokens[i][j].type == TokenType::L_SQBACKET) {
              _j++;
              int startPos = _j;
              while (tokens[i][_j].type != TokenType::R_SQBACKET) {
                _j++;
                if (_j >= tokens[i].size()) {
                  error(ERROR::BRACKET, i, j);
                }
              }
              index = doMath<int>(i, startPos, i, _j);
            } else {
              error(ERROR::ARRAY_ADDING, i, j);
            }
          }
          tokens[i][j].value = _var->getvalue(index);
          j--;
        }
          continue;
        case TokenType::NUMBER:
          str += tokens[i][j].value;
          if (j + 1 < tokens[i].size()) {
            if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                j++;
              }
            }
          }
          continue;
        default:
          str += tokens[i][j].value;
          continue;
      }
    }
  }
  if constexpr (is_same_v<T, long double>) {
    Func func(str);
    var = func.get_y();
  } else if constexpr (is_same_v<T, string>) {
    for (int i = line; i <= end_line; i++) {
      for (int j = 0; j < tokens[i].size(); j++) {
        if (tokens[i][j].type == TokenType::TRUESTRING) {
          var += tokens[i][j].value;
        } else if (tokens[i][j].type == TokenType::PLUS) {
          continue;
        }
      }
    }
  } else {
    error(ERROR::OTHER, -1, -1);
  }
  return var;
}