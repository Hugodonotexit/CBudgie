#include "parser.h"
Parser_main::~Parser_main() {}
Parser_main::Parser_main(vector<vector<Token>> &token) : MathSupport(token) {
  #pragma omp parallel for
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

bool Parser_main::runFunction(int index) { return runFunction(index, -1, -1); }

bool Parser_main::runFunction(int index, int _line_, int _pos_) {
  vector<shared_ptr<Variable>> var;
  return runFunction(index, -1, -1, var);
}
bool Parser_main::runFunction(int index, vector<shared_ptr<Variable>> var) {
  return runFunction(index, -1, -1, var);
}
bool Parser_main::runFunction(int index, int _line_, int _pos_,
                              vector<shared_ptr<Variable>> var) {
  int varStartSize = variable.size();
  int scopeStartSize = scope.size();
  const Scope::Location &startPos = scope[index]->getStartPos();
  const Scope::Location &endPos = scope[index]->getEndPos();
  Scope::Location conStartPos = scope[index]->getConStartPos();
  Scope::Location conEndPos = scope[index]->getConEndPos();
  bool isRun = false;
  bool stop = false;
  if (!var.empty()) {
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
  }
  for (int i = startPos.line; i <= endPos.line; i++) {
    if (stop) break;
#pragma omp parallel for
    for (int j = 0; j < tokens[i].size(); j++) {
      if (i == startPos.line && j < startPos.pos) {
        j = startPos.pos;
      } else if (i == endPos.line && j > endPos.pos) {
        stop = true;
        break;
      }
      // cerr << "At run;" << i << ":" << j << endl;
      switch (tokens[i][j].type) {
        case TokenType::LET:
          defVar(i, j, endPos.line, endPos.pos);
          break;
        case TokenType::COUT:
          coutConfig(i, j);
          break;
        case TokenType::FUNCTIONISED:
          if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
            if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[i][j].value), i, j);
            } else {
              int start = j++;
              vector<shared_ptr<Variable>> passingVars;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                switch (tokens[i][j].type) {
                  case TokenType::VARIABLISED_BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[i][j].value)]));
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
                    var.newvalue(doMath<long double>(i, j, i, j));
                    passingVars.push_back(
                        make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.newvalue(doMath<string>(i, j, i, j));
                    passingVars.push_back(
                        make_shared<VariableType<string>>(var));
                  } break;
                }
                j++;
              }
              runFunction(stoi(tokens[i][start].value), i, start, passingVars);
              j--;
            }
          } else {
            error(ERROR::BRACKET, i, j + 1);
          }
        case TokenType::CONTINUE:
          return false;
        case TokenType::BREAK:
          return true;
        case TokenType::RETURN:
          switch (tokens[i][++j].type) {
            case TokenType::VARIABLISED_BOOL: {
              tokens[_line_][_pos_].type = TokenType::VARIABLISED_BOOL;
              tokens[_line_][_pos_].value = tokens[i][j].value;
            } break;
            case TokenType::TRUE:
              tokens[_line_][_pos_].type = TokenType::TRUE;
              return false;
            case TokenType::FALSE:
              tokens[_line_][_pos_].type = TokenType::FALSE;
              return false;
            case TokenType::NUMBER:
            case TokenType::VARIABLISED_NUM:
              tokens[_line_][_pos_].type = TokenType::NUMBER;
              tokens[_line_][_pos_].value =
                  to_string(doMath<long double>(i, j, i, tokens[i].size() - 1));
              return false;
            case TokenType::TRUESTRING:
            case TokenType::VARIABLISED_STR:
              tokens[_line_][_pos_].type = TokenType::TRUESTRING;
              tokens[_line_][_pos_].value =
                  doMath<string>(i, j, i, tokens[i].size() - 1);
              return false;
            case TokenType::FUNCTIONISED:
              if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
                if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
                  runFunction(stoi(tokens[i][j].value), i, j);
                } else {
                  int start = j++;
                  vector<shared_ptr<Variable>> vars;
                  while (tokens[i][j].type != TokenType::R_RBACKET) {
                    switch (tokens[i][j].type) {
                      case TokenType::VARIABLISED_BOOL: {
                        VariableType<bool> var;
                        var = *(dynamic_pointer_cast<VariableType<bool>>(
                            variable[stoi(tokens[i][j].value)]));
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
                        var.newvalue(doMath<long double>(i, j, i, j));
                        vars.push_back(
                            make_shared<VariableType<long double>>(var));
                      } break;
                      case TokenType::TRUESTRING:
                      case TokenType::VARIABLISED_STR: {
                        VariableType<string> var;
                        var.newvalue(doMath<string>(i, j, i, j));
                        vars.push_back(make_shared<VariableType<string>>(var));
                      } break;
                    }
                    j++;
                  }
                  runFunction(stoi(tokens[i][start].value), i, start, vars);
                  j--;
                }
              } else {
                error(ERROR::BRACKET, i, j + 1);
              }
              continue;
            case TokenType::STRING:
              error(ERROR::UNDEF, i, j);
              break;
            default:
              error(ERROR::OTHER, i, j);
              break;
          }
        case TokenType::EQUAL:
          switch (tokens[i][j - 1].type) {
            case TokenType::VARIABLISED_NUM: {
              shared_ptr<VariableType<long double>> var =
                  dynamic_pointer_cast<VariableType<long double>>(
                      variable[stoi(tokens[i][j - 1].value)]);
              j++;
              var->newvalue(doMath<long double>(i, j, i, tokens[i].size() - 1));
            } break;
            case TokenType::VARIABLISED_STR: {
              shared_ptr<VariableType<string>> var =
                  dynamic_pointer_cast<VariableType<string>>(
                      variable[stoi(tokens[i][j - 1].value)]);
              j++;
              var->newvalue(doMath<string>(i, j, i, tokens[i].size() - 1));
            } break;
            case TokenType::VARIABLISED_BOOL: {
              shared_ptr<VariableType<bool>> var =
                  dynamic_pointer_cast<VariableType<bool>>(
                      variable[stoi(tokens[i][j - 1].value)]);
              j++;
              var->newvalue(boolOP(i, j, i, tokens[i].size() - 1));
            } break;
            case TokenType::STRING:
              error(ERROR::UNDEF, i, j - 1);
              break;
            default:
              error(ERROR::OTHER, i, j - 1);
              break;
          }
          break;
        case TokenType::IF: {
          bool runningIF = false;
          if (j - 1 >= 0) {
            if ((tokens[i][j - 1].type == TokenType::ELSE && !isRun) ||
                tokens[i][j - 1].type != TokenType::ELSE) {
              runningIF = true;
            }
          } else if ((tokens[i - 1][tokens[i - 1].size() - 1].type ==
                          TokenType::ELSE &&
                      !isRun) ||
                     tokens[i - 1][tokens[i - 1].size() - 1].type !=
                         TokenType::ELSE) {
            runningIF = true;
          }
          // cerr << i << ":" << j << ";" << runningIF << endl;
          int indeX = defScope<If>(i, j);
          shared_ptr<If> thisScope = dynamic_pointer_cast<If>(scope[indeX]);
          int startLINE = thisScope->getConStartPos().line;
          int startPos = thisScope->getConStartPos().pos;
          int endLINE = thisScope->getConEndPos().line;
          int endPos = thisScope->getConEndPos().pos;
          if (runningIF) {
            isRun = boolOP(startLINE, startPos, endLINE, endPos);
          }

          if (isRun && runningIF) {
            runFunction(indeX);
          }

          i = thisScope->getEndPos().line;
          j = thisScope->getEndPos().pos;
        }
          continue;
        case TokenType::ELSE: {
          if (j + 1 < tokens[i].size()) {
            if (tokens[i][j + 1].type == TokenType::IF) {
              continue;
            }
          } else if (tokens[i + 1][0].type == TokenType::IF) {
            continue;
          }
          int indeX = defScope<Else>(i, j);
          shared_ptr<Else> thisScope = dynamic_pointer_cast<Else>(scope[indeX]);
          if (!isRun) {
            runFunction(indeX);
          }
          isRun = false;
          i = thisScope->getEndPos().line;
          j = thisScope->getEndPos().pos;
        }
          continue;
        case TokenType::FOR: {
          if (j < tokens[i].size()) {
            if (tokens[i][j + 1].type != TokenType::L_RBACKET) {
              error(ERROR::BRACKET, i, j);
            }
          }
          int thisScopeIndex = defScope<For>(i, j);
          shared_ptr<For> thisScope =
              dynamic_pointer_cast<For>(scope[thisScopeIndex]);
          int lineSec[4], posSec[4], colonNum = 0;
          lineSec[0] = thisScope->getConStartPos().line;
          posSec[0] = thisScope->getConStartPos().pos + 1;
          int lineEnd = thisScope->getEndPos().line;
          int posEnd = thisScope->getEndPos().pos;
          int thisVarIndex = defVar(lineSec[0], posSec[0], lineEnd, posEnd);
          shared_ptr<VariableType<long double>> var =
              dynamic_pointer_cast<VariableType<long double>>(
                  variable[thisVarIndex]);
          while (colonNum < 3) {
            switch (tokens[i][++j].type) {
              case TokenType::R_RBACKET:
                colonNum++;
                lineSec[colonNum] = i;
                posSec[colonNum] = j;
                stop = true;
                break;
              case TokenType::COLON:
                colonNum++;
                lineSec[colonNum] = i;
                posSec[colonNum] = j;
                break;
            }
            if (stop) break;
          }
          switch (colonNum) {
            case 0:
            case 1:
              error(ERROR::INIT_VAR, i, j);
              break;
            case 2: {
              int POS = posSec[1] + 1;
              var->newMaxValue(doMath<long double>(lineSec[1], POS, lineSec[2],
                                                   posSec[2] - 1));
              var->newStep(1);
            } break;
            case 3: {
              int _POS = posSec[1] + 1;
              int POS_ = posSec[2] + 1;
              var->newMaxValue(doMath<long double>(lineSec[1], _POS, lineSec[2],
                                                   posSec[2] - 1));
              var->newStep(doMath<long double>(lineSec[2], POS_, lineSec[3],
                                               posSec[3] - 1));
            } break;
          }
          if (var->getStep() > 0 && var->getMaxValue() >= var->getvalue(0)) {
            for (long double &_i_ = var->getvalue(0); _i_ <= var->getMaxValue();
                 _i_ = _i_ + var->getStep()) {
              if (runFunction(thisScopeIndex)) {
                return false;
              }
            }
          } else if (var->getStep() < 0 &&
                     var->getMaxValue() <= var->getvalue(0)) {
            for (long double &_i_ = var->getvalue(0); _i_ >= var->getMaxValue();
                 _i_ = _i_ + var->getStep()) {
              if (runFunction(thisScopeIndex)) {
                return false;
              }
            }
          } else {
            error(ERROR::OTHER, i, j);
          }
          i = thisScope->getConEndPos().line;
          j = thisScope->getConEndPos().pos;
        }
          continue;
        case TokenType::WHILE: {
          // cerr << i << ":" << j << ";" << runningIF << endl;
          if (tokens[i][j + 1].type != TokenType::L_RBACKET) {
            error(ERROR::BRACKET, i, j + 1);
          }
          int thisScopeIndex = defScope<While>(i, j);
          shared_ptr<If> thisScope =
              dynamic_pointer_cast<If>(scope[thisScopeIndex]);
          int startLINE = thisScope->getConStartPos().line;
          int startPos = thisScope->getConStartPos().pos;
          int endLINE = thisScope->getConEndPos().line;
          int endPos = thisScope->getConEndPos().pos;
          while (boolOP(startLINE, startPos, endLINE, endPos)) {
            runFunction(thisScopeIndex);
            startLINE = thisScope->getConStartPos().line;
            startPos = thisScope->getConStartPos().pos;
          }
          i = thisScope->getEndPos().line;
          j = thisScope->getEndPos().pos;
        }
          continue;
        default:
          continue;
      }
    }
  }
  if (variable.size() > varStartSize) {
    variable.erase(variable.begin() + varStartSize, variable.end());
  }
  if (scope.size() > scopeStartSize) {
    scope.erase(scope.begin() + scopeStartSize, scope.end());
  }
  return false;
}
int Parser_main::defVar(int &line, int &pos, int end_line, int end_pos) {
  vector<shared_ptr<Variable>> pass;
  return defVar(line, pos, end_line, end_pos, pass);
}
int Parser_main::defVar(int &line, int &pos, int end_line, int end_pos,
                        vector<shared_ptr<Variable>> pass) {
  shared_ptr<Variable> var;
  TokenType type;
  size_t size = 1;
  int _pos;
  int _line;
  string name;
  // cerr << line << ":" << pos << ";" << end_line << ":" << end_pos << endl;
  if (pass.empty()) {
    _pos = pos++;
    _line = line;
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
    if (tokens[line][pos + 1].type == TokenType::L_SBACKET) {
      pos++;
    }
    pos++;
    switch (tokens[line][pos].type) {
      case TokenType::STRING:
        error(ERROR::UNDEF, line, pos);
        break;
      case TokenType::VARIABLISED_STR:
      case TokenType::TRUESTRING: {
        type = TokenType::VARIABLISED_STR;
        vector<string> array;
        bool pass = false;
        while (pos < tokens[line].size()) {
          if (pos + 1 < tokens[line].size()) {
            if (tokens[line][pos + 1].type == TokenType::L_RBACKET) {
              pass = true;
            }
          } else if (tokens[line][pos].type == TokenType::R_SQBACKET) {
            pos++;
            break;
          }
          array.push_back(
              doMath<string>(line, pos, line, tokens[line].size() - 1));
          if (pass) break;
          pos++;
        }
        if (!pass) pos--;
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        while (pass && tokens[line][pos].type != TokenType::R_RBACKET) {
          pos++;
          if (pos == tokens[line].size()) {
            error(ERROR::BRACKET, line, pos);
          }
        }
        var = make_shared<VariableType<string>>(name, array);
      } break;
      case TokenType::VARIABLISED_NUM:
      case TokenType::NUMBER: {
        type = TokenType::VARIABLISED_NUM;
        vector<long double> array;
        bool pass = false;
        while (pos < tokens[line].size()) {
          if (pos + 1 < tokens[line].size()) {
            if (tokens[line][pos + 1].type == TokenType::L_RBACKET) {
              pass = true;
            }
          } else if (tokens[line][pos].type == TokenType::R_SQBACKET) {
            pos++;
            break;
          }
          array.push_back(
              doMath<long double>(line, pos, line, tokens[line].size() - 1));
          if (pass) break;
          pos++;
        }
        if (!pass) pos--;
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        while (pass && tokens[line][pos].type != TokenType::R_RBACKET) {
          pos++;
          if (pos == tokens[line].size()) {
            error(ERROR::BRACKET, line, pos);
          }
        }
        var = make_shared<VariableType<long double>>(name, array);
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
            int start_line = line;
            int start_pos = pos++;
            vector<shared_ptr<Variable>> vars;
            while (tokens[line][pos].type != TokenType::R_RBACKET) {
              switch (tokens[line][pos].type) {
                case TokenType::VARIABLISED_BOOL: {
                  VariableType<bool> var;
                  var = *(dynamic_pointer_cast<VariableType<bool>>(
                      variable[stoi(tokens[line][pos].value)]));
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
                  var.newvalue(doMath<long double>(line, pos, line, pos));
                  vars.push_back(make_shared<VariableType<long double>>(var));
                } break;
                case TokenType::TRUESTRING:
                case TokenType::VARIABLISED_STR: {
                  VariableType<string> var;
                  var.newvalue(doMath<string>(line, pos, line, pos));
                  vars.push_back(make_shared<VariableType<string>>(var));
                } break;
              }
              pos++;
            }
            runFunction(stoi(tokens[start_line][start_pos].value), start_line,
                        start_pos, vars);
            pos--;
          }

        } else {
          error(ERROR::BRACKET, line, pos + 1);
        }
        return defVar(_line, _pos, end_line, end_pos);
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
  return variable.size() - 1;
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
int Parser_main::defScope(int line, int pos) {
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
  return (scope.size() - 1);
};

template <typename T>
T Parser_main::doMath(int &line, int &pos, int end_line, int end_pos) {
  T var;
  int _line_ = line;
  int _pos_ = pos;
  bool stop = false;
  vector<int> changedLine, changedPos;
  vector<string> changedIndex;
  vector<int> changedLine_str, changedPos_str;
  vector<string> changedIndex_str;
  vector<int> changedLine_fun, changedPos_fun;
  vector<string> changedIndex_fun;
  for (int i = line; i <= end_line; i++) {
    if (stop) {
      break;
    }
    for (int j = 0; j < tokens[i].size(); j++) {
      T localVar;
      if (i == line && j < pos) {
        j = pos;
      } else if ((i >= end_line && j > end_pos) ||
                 tokens[i][j].type == TokenType::COMMA) {
        stop = true;
        break;
      }
      _line_ = i;
      _pos_ = j;
      switch (tokens[i][j].type) {
        case TokenType::CIN:
          if constexpr (is_same_v<T, string>) {
            coutConfig(i, j, false);
            string str;
            line = i;
            pos = j;
            str = Consule::input();
            return str;
          } else if constexpr (is_same_v<T, long double>) {
            coutConfig(i, j, false);
            string str;
            line = i;
            pos = j;
            str = Consule::input();
            return stold(str);
          }
        case TokenType::FUNCTIONISED:
          changedLine_fun.push_back(i);
          changedPos_fun.push_back(j);
          changedIndex_fun.push_back(tokens[i][j].value);
          if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
            if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[i][j].value), i, j);
            } else {
              int start = j++;
              vector<shared_ptr<Variable>> vars;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                // cerr << "AT: " << i << ";" << j << endl;
                switch (tokens[i][j].type) {
                  case TokenType::VARIABLISED_BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[i][j].value)]));
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
                    var.pushValue(doMath<long double>(i, j, i, j));
                    vars.push_back(make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.pushValue(doMath<string>(i, j, i, j));
                    vars.push_back(make_shared<VariableType<string>>(var));
                  } break;
                }
                j++;
              }
              runFunction(stoi(tokens[i][start].value), i, start, vars);
              j = start;
              j--;
            }
          } else {
            error(ERROR::BRACKET, i, j + 1);
          }
          continue;
        case TokenType::VARIABLISED_NUM: {
          changedLine.push_back(i);
          changedPos.push_back(j);
          changedIndex.push_back(tokens[i][j].value);
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
          changedLine_str.push_back(i);
          changedPos_str.push_back(j);
          changedIndex_str.push_back(tokens[i][j].value);
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
          if (j + 1 < tokens[i].size()) {
            if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                j++;
              }
              j++;
            }
          }
          continue;
      }
    }
  }
  if constexpr (is_same_v<T, long double>) {
    var = mathOP(line, pos, end_line, end_pos);
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
    error(ERROR::OTHER, line, pos);
  }
#pragma omp parallel for
  for (int i = 0; i < changedIndex_fun.size(); i++) {
    tokens[changedLine_fun[i]][changedPos_fun[i]].type =
        TokenType::FUNCTIONISED;
    tokens[changedLine_fun[i]][changedPos_fun[i]].value = changedIndex_fun[i];
  }
#pragma omp parallel for
  for (int i = 0; i < changedIndex_str.size(); i++) {
    tokens[changedLine_str[i]][changedPos_str[i]].type =
        TokenType::VARIABLISED_STR;
    tokens[changedLine_str[i]][changedPos_str[i]].value = changedIndex_str[i];
  }
#pragma omp parallel for
  for (int i = 0; i < changedIndex.size(); i++) {
    tokens[changedLine[i]][changedPos[i]].type = TokenType::VARIABLISED_NUM;
    tokens[changedLine[i]][changedPos[i]].value = changedIndex[i];
  }
  line = _line_;
  pos = _pos_;
  return var;
}

bool Parser_main::boolOP(int &line, int &pos, int end_line, int end_pos) {
  tokens_copy.erase(tokens_copy.begin(), tokens_copy.end());
  bool stopping = false;
  vector<int> changedLine, changedPos;
  vector<string> changedIndex;
  tokens_copy.emplace_back(TokenType::L_RBACKET, "(");
  for (int i = line; i <= end_line; i++) {
    if (stopping) break;
    for (int j = 0; j < tokens[i].size(); j++) {
      if (i == line && j < pos) {
        j = pos;
      } else if (i >= end_line && j > end_pos) {
        stopping = true;
        break;
      }
      switch (tokens[i][j].type) {
        case TokenType::VARIABLISED_BOOL: {
          changedLine.push_back(i);
          changedPos.push_back(j);
          changedIndex.push_back(tokens[i][j].value);
          shared_ptr<VariableType<bool>> var =
              dynamic_pointer_cast<VariableType<bool>>(
                  variable[stoi(tokens[i][j].value)]);
          if (var->get_value(0)) {
            tokens_copy.emplace_back(TokenType::TRUE, "true");
          } else {
            tokens_copy.emplace_back(TokenType::FALSE, "false");
          }
        }
          continue;
        case TokenType::VARIABLISED_NUM: {
          tokens_copy.emplace_back(TokenType::NUMBER,
                                   to_string(doMath<long double>(i, j, i, j)));
        }
          continue;
        case TokenType::VARIABLISED_STR: {
          VariableType<string> var;
          tokens_copy.emplace_back(TokenType::TRUESTRING,
                                   doMath<string>(i, j, i, j));
        }
          continue;
        case TokenType::FUNCTIONISED: {
          if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
            if (tokens[i][j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[i][j].value), i, j);
            } else {
              int start = j++;
              vector<shared_ptr<Variable>> vars;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                switch (tokens[i][j].type) {
                  case TokenType::VARIABLISED_BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[i][j].value)]));
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
                    var.newvalue(doMath<long double>(i, j, i, j));
                    vars.push_back(make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.newvalue(doMath<string>(i, j, i, j));
                    vars.push_back(make_shared<VariableType<string>>(var));
                  } break;
                }
                j++;
              }
              runFunction(stoi(tokens[i][start].value), i, start, vars);
              j--;
            }
          } else {
            error(ERROR::BRACKET, i, j + 1);
          }
        }
          continue;
        case TokenType::NUMBER:
          tokens_copy.push_back(tokens[i][j]);
          if (j + 1 < tokens[i].size()) {
            if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                j++;
              }
              j++;
            }
          }
          continue;
        default: {
          tokens_copy.push_back(tokens[i][j]);
        }
          continue;
      }
    }
  }
  tokens_copy.emplace_back(TokenType::R_RBACKET, ")");
  while (tokens_copy.size() != 1) {
    scanFunc();
    if (!brackets.empty()) {
      for (int j = brackets.size() - 1; j >= 0; j--) {
        for (int i = (int)brackets[j].first + 1;
             i < (int)brackets[j].second - 1; i++) {
          for (int k = 0; k < (int)operators[0].size(); k++) {
            if (i == operators[0][k]) {
              calculate(i);
              scanFunc();
            }
          }
        }

        bool foundInOperators0 = false;
        #pragma omp parallel for
        for (int i = (int)brackets[j].first + 1;
             i < (int)brackets[j].second - 1; i++) {
          #pragma omp parallel for
          for (int k = 0; k < (int)operators[0].size(); k++) {
            if (i == operators[0][k]) {
              foundInOperators0 = true;
              break;
            }
          }
          if (foundInOperators0) {
            break;
          }
        }

        if (!foundInOperators0) {
          for (int i = (int)brackets[j].first + 1;
               i < (int)brackets[j].second - 1; i++) {
            for (int k = 0; k < (int)operators[1].size(); k++) {
              if (i == operators[1][k]) {
                calculate(i);
                scanFunc();
              }
            }
          }
        }
      }
    }
  }
#pragma omp parallel for
  for (int i = 0; i < changedIndex.size(); i++) {
    tokens[changedLine[i]][changedPos[i]].type = TokenType::FUNCTIONISED;
    tokens[changedLine[i]][changedPos[i]].value = changedIndex[i];
  }
  line = end_line;
  pos = end_pos;
  if (tokens_copy[0].type == TokenType::TRUE) {
    return true;
  }
  return false;
}

long double Parser_main::mathOP(int &line, int &pos, int end_line,
                                int end_pos) {
  // cerr << line << ":" << pos << ";" << end_line << ":" << end_pos << endl;
  math_tokens_copy.erase(math_tokens_copy.begin(), math_tokens_copy.end());
  bool stopping = false;
  math_tokens_copy.emplace_back(TokenType::L_RBACKET, "(");
  for (int i = line; i <= end_line; i++) {
    if (stopping) break;
    for (int j = 0; j < tokens[i].size(); j++) {
      if (i == line && j < pos) {
        j = pos;
      } else if ((i >= end_line && j > end_pos) || stopping) {
        stopping = true;
        break;
      }
      // cerr << i << ":" << j << endl;
      switch (tokens[i][j].type) {
        case TokenType::COMMA:
        case TokenType::COLON:
          stopping = true;
          break;
        case TokenType::NUMBER:
          math_tokens_copy.push_back(tokens[i][j]);
          if (j + 1 < tokens[i].size()) {
            if (tokens[i][j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[i][j].type != TokenType::R_RBACKET) {
                j++;
              }
              j++;
            }
          }
          continue;
        default:
          math_tokens_copy.push_back(tokens[i][j]);
          continue;
      }
    }
  }
  math_tokens_copy.emplace_back(TokenType::R_RBACKET, ")");

  /*for (int i = 0; i < math_tokens_copy.size(); i++)
  {
    cerr << math_tokens_copy[i].value << " ";
  }
  cerr << endl;*/

  math_scanFunc();
  while (math_tokens_copy.size() != 1) {
    math_scanFunc();
    if (!math_brackets.empty()) {
      for (int j = math_brackets.size() - 1; j >= 0; j--) {
        for (int i = (int)math_brackets[j].first + 1;
             i < (int)math_brackets[j].second - 1; i++) {
          for (int k = 0; k < (int)math_operators[0].size(); k++) {
            if (i == math_operators[0][k]) {
              math_calculate(i);
              math_scanFunc();
            }
          }
        }

        bool foundInOperators0 = false;
        #pragma omp parallel for
        for (int i = (int)math_brackets[j].first + 1;
             i < (int)math_brackets[j].second - 1; i++) {
              #pragma omp parallel for
          for (int k = 0; k < (int)math_operators[0].size(); k++) {
            if (i == math_operators[0][k]) {
              foundInOperators0 = true;
              break;
            }
          }
          if (foundInOperators0) {
            break;
          }
        }

        if (!foundInOperators0) {
          for (int i = (int)math_brackets[j].first + 1;
               i < (int)math_brackets[j].second - 1; i++) {
            for (int k = 0; k < (int)math_operators[1].size(); k++) {
              if (i == math_operators[1][k]) {
                math_calculate(i);
                math_scanFunc();
              }
            }
          }
          bool foundInOperators1 = false;
          #pragma omp parallel for
          for (int i = (int)math_brackets[j].first + 1;
               i < (int)math_brackets[j].second - 1; i++) {
                #pragma omp parallel for
            for (int k = 0; k < (int)math_operators[1].size(); k++) {
              if (i == math_operators[1][k]) {
                foundInOperators1 = true;
                break;
              }
            }
            if (foundInOperators1) {
              break;
            }
          }

          if (!foundInOperators1) {
            for (int i = (int)math_brackets[j].first + 1;
                 i < (int)math_brackets[j].second - 1; i++) {
              for (int k = 0; k < (int)math_operators[2].size(); k++) {
                if (i == math_operators[2][k]) {
                  math_calculate(i);
                  math_scanFunc();
                }
              }
            }
          }
        }
      }
    }
  }
  line = end_line;
  pos = end_pos;
  return stold(math_tokens_copy[0].value);
}