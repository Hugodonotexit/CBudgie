#include "parser.h"
Parser_main::~Parser_main() {}
Parser_main::Parser_main(vector<Token> token,map<int,int> &range) : MathSupport(token,range) {
    for (int pos = 0; pos < tokens.size(); pos++) {
      switch (tokens[pos].type) {
        case TokenType::FILE:
          defFile(pos);
          break;
        case TokenType::DEF:
          defScope<Function>(pos);
          break;
      }
    }
  runFunction(mainIndex);
}

bool Parser_main::runFunction(int index, int _pos_, vector<shared_ptr<Variable>> var) {
  int varStartSize = variable.size();
  int scopeStartSize = scope.size();
  int startPos = scope[index]->getStartPos();
  int endPos = scope[index]->getEndPos();
  int conStartPos = scope[index]->getConStartPos();
  int conEndPos = scope[index]->getConEndPos();
  bool isRun = false;
  if (!var.empty()) {
    if (conEndPos - conStartPos != 1) {
      for (int i = conStartPos + 1; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::LET) {
          defVar(i, endPos, var);
          var.erase(var.begin());
        } else if (tokens[i].type == TokenType::R_RBACKET) {
          break;
        }
      }
    }
  }
    for (int j = startPos; j < endPos; j++) {
      // cerr << "At run;" << j << endl;
      switch (tokens[j].type) {
        case TokenType::LET:
          defVar(j, endPos);
          break;
        case TokenType::COUT:
          coutConfig(j);
          break;
        case TokenType::FUNCTIONISED:
          if (tokens[j + 1].type == TokenType::L_RBACKET) {
            if (tokens[j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[j].value), j);
            } else {
              int start = j++;
              vector<shared_ptr<Variable>> passingVars;
              while (tokens[j].type != TokenType::R_RBACKET) {
                switch (tokens[j].type) {
                  case TokenType::VARIABLISED_BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[j].value)]));
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
                    var.newvalue(doMath<long double>(j, j));
                    passingVars.push_back(
                        make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.newvalue(doMath<string>(j, j));
                    passingVars.push_back(
                        make_shared<VariableType<string>>(var));
                  } break;
                }
                j++;
              }
              runFunction(stoi(tokens[start].value), start, passingVars);
              j--;
            }
          } else {
            error(ERROR::BRACKET, j + 1);
          }
        case TokenType::CONTINUE:
          return false;
        case TokenType::BREAK:
          return true;
        case TokenType::RETURN:
          switch (tokens[++j].type) {
            case TokenType::VARIABLISED_BOOL: {
              tokens[_pos_].type = TokenType::VARIABLISED_BOOL;
              tokens[_pos_].value = tokens[j].value;
            } break;
            case TokenType::TRUE:
              tokens[_pos_].type = TokenType::TRUE;
              return false;
            case TokenType::FALSE:
              tokens[_pos_].type = TokenType::FALSE;
              return false;
            case TokenType::NUMBER:
            case TokenType::VARIABLISED_NUM:
              tokens[_pos_].type = TokenType::NUMBER;
              tokens[_pos_].value =
                  to_string(doMath<long double>(j, tokens.size() - 1));
              return false;
            case TokenType::TRUESTRING:
            case TokenType::VARIABLISED_STR:
              tokens[_pos_].type = TokenType::TRUESTRING;
              tokens[_pos_].value =
                  doMath<string>(j, tokens.size() - 1);
              return false;
            case TokenType::FUNCTIONISED:
              if (tokens[j + 1].type == TokenType::L_RBACKET) {
                if (tokens[j + 2].type == TokenType::R_RBACKET) {
                  runFunction(stoi(tokens[j].value), j);
                } else {
                  int start = j++;
                  vector<shared_ptr<Variable>> vars;
                  while (tokens[j].type != TokenType::R_RBACKET) {
                    switch (tokens[j].type) {
                      case TokenType::VARIABLISED_BOOL: {
                        VariableType<bool> var;
                        var = *(dynamic_pointer_cast<VariableType<bool>>(
                            variable[stoi(tokens[j].value)]));
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
                        var.newvalue(doMath<long double>(j, j));
                        vars.push_back(
                            make_shared<VariableType<long double>>(var));
                      } break;
                      case TokenType::TRUESTRING:
                      case TokenType::VARIABLISED_STR: {
                        VariableType<string> var;
                        var.newvalue(doMath<string>(j, j));
                        vars.push_back(make_shared<VariableType<string>>(var));
                      } break;
                    }
                    j++;
                  }
                  runFunction(stoi(tokens[start].value), start, vars);
                  j--;
                }
              } else {
                error(ERROR::BRACKET, j + 1);
              }
              continue;
            case TokenType::STRING:
              error(ERROR::UNDEF, j);
              break;
            default:
              error(ERROR::OTHER, j);
              break;
          }
        case TokenType::EQUAL:
          switch (tokens[j - 1].type) {
            case TokenType::VARIABLISED_NUM: {
              shared_ptr<VariableType<long double>> var =
                  dynamic_pointer_cast<VariableType<long double>>(
                      variable[stoi(tokens[j - 1].value)]);
              j++;
              var->newvalue(doMath<long double>(j, tokens.size() - 1));
            } break;
            case TokenType::VARIABLISED_STR: {
              shared_ptr<VariableType<string>> var =
                  dynamic_pointer_cast<VariableType<string>>(
                      variable[stoi(tokens[j - 1].value)]);
              j++;
              var->newvalue(doMath<string>(j, tokens.size() - 1));
            } break;
            case TokenType::VARIABLISED_BOOL: {
              shared_ptr<VariableType<bool>> var =
                  dynamic_pointer_cast<VariableType<bool>>(
                      variable[stoi(tokens[j - 1].value)]);
              j++;
              var->newvalue(boolOP(j, tokens.size() - 1));
            } break;
            case TokenType::STRING:
              error(ERROR::UNDEF, j - 1);
              break;
            default:
              error(ERROR::OTHER, j - 1);
              break;
          }
          break;
        case TokenType::IF: {
          bool runningIF = false;
            if ((tokens[j - 1].type == TokenType::ELSE && !isRun) ||
                tokens[j - 1].type != TokenType::ELSE) {
              runningIF = true;
            }
          // cerr << i << ":" << j << ";" << runningIF << endl;
          int indeX = defScope<If>(j);
          shared_ptr<If> thisScope = dynamic_pointer_cast<If>(scope[indeX]);
          int startPos = thisScope->getConStartPos();
          int endPos = thisScope->getConEndPos();
          if (runningIF) {
            isRun = boolOP(startPos, endPos);
          }
          if (isRun && runningIF) {
            runFunction(indeX);
          }

          j = thisScope->getEndPos();
        }
          continue;
        case TokenType::ELSE: {
            if (tokens[j + 1].type == TokenType::IF) continue;
          int indeX = defScope<Else>(j);
          shared_ptr<Else> thisScope = dynamic_pointer_cast<Else>(scope[indeX]);
          if (!isRun) runFunction(indeX);
          isRun = false;
          j = thisScope->getEndPos();
        }
          continue;
        case TokenType::FOR: {
          if (j < tokens.size()) {
            if (tokens[j + 1].type != TokenType::L_RBACKET) {
              error(ERROR::BRACKET, j);
            }
          }
          int thisScopeIndex = defScope<For>(j);
          shared_ptr<For> thisScope =
              dynamic_pointer_cast<For>(scope[thisScopeIndex]);
          int posSec[4], colonNum = 0;
          posSec[0] = thisScope->getConStartPos() + 1;
          int posEnd = thisScope->getEndPos();
          int thisVarIndex = defVar(posSec[0], posEnd);
          shared_ptr<VariableType<long double>> var =
              dynamic_pointer_cast<VariableType<long double>>(
                  variable[thisVarIndex]);
          bool stop = false;
          while (colonNum < 3) {
            switch (tokens[++j].type) {
              case TokenType::R_RBACKET:
                colonNum++;
                posSec[colonNum] = j;
                stop = true;
                break;
              case TokenType::COLON:
                colonNum++;
                posSec[colonNum] = j;
                break;
            }
            if (stop) break;
          }
          switch (colonNum) {
            case 0:
            case 1:
              error(ERROR::INIT_VAR, j);
              break;
            case 2: {
              int POS = posSec[1] + 1;
              var->newMaxValue(doMath<long double>(POS, posSec[2] - 1));
              var->newStep(1);
            } break;
            case 3: {
              int _POS = posSec[1] + 1;
              int POS_ = posSec[2] + 1;
              var->newMaxValue(doMath<long double>(_POS, posSec[2] - 1));
              var->newStep(doMath<long double>(POS_, posSec[3] - 1));
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
            error(ERROR::OTHER, j);
          }
          j = thisScope->getConEndPos();
        }
          continue;
        case TokenType::WHILE: {
          // cerr << i << ":" << j << ";" << runningIF << endl;
          if (tokens[j + 1].type != TokenType::L_RBACKET) {
            error(ERROR::BRACKET, j + 1);
          }
          int thisScopeIndex = defScope<While>(j);
          shared_ptr<If> thisScope =
              dynamic_pointer_cast<If>(scope[thisScopeIndex]);
          int startPos = thisScope->getConStartPos();
          int endPos = thisScope->getConEndPos();
          while (boolOP(startPos, endPos)) {
            runFunction(thisScopeIndex);
            startPos = thisScope->getConStartPos();
          }
          j = thisScope->getEndPos();
        }
          continue;
        default:
          continue;
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

int Parser_main::defVar(int &pos, int end_pos, vector<shared_ptr<Variable>> pass) {
  shared_ptr<Variable> var;
  TokenType type;
  size_t size = 1;
  int _pos;
  string name;
  // cerr << line << ":" << pos << ";" << end_line << ":" << end_pos << endl;
  if (pass.empty()) {
    _pos = pos++;
    name = tokens[pos].value;
    if (tokens[pos + 1].type == TokenType::L_SQBACKET) {
      if (tokens[pos + 3].type == TokenType::R_SQBACKET) {
        size = stoi(tokens[pos + 2].value);
        pos += 3;
      } else {
        error(ERROR::BRACKET, pos + 4);
      }
    }
    pos++;
  }
  if (tokens[pos].type == TokenType::EQUAL) {
    if (tokens[pos + 1].type == TokenType::L_SBACKET) {
      pos++;
    }
    pos++;
    switch (tokens[pos].type) {
      case TokenType::STRING:
        error(ERROR::UNDEF, pos);
        break;
      case TokenType::VARIABLISED_STR:
      case TokenType::TRUESTRING: {
        type = TokenType::VARIABLISED_STR;
        vector<string> array;
        bool pass = false;
        while (pos < tokens.size()) {
          if (pos + 1 < tokens.size()) {
            if (tokens[pos + 1].type == TokenType::L_RBACKET) {
              pass = true;
            }
          } else if (tokens[pos].type == TokenType::R_SQBACKET) {
            pos++;
            break;
          }
          array.push_back(
              doMath<string>(pos, tokens.size() - 1));
          if (pass) break;
          pos++;
        }
        if (!pass) pos--;
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        while (pass && tokens[pos].type != TokenType::R_RBACKET) {
          pos++;
          if (pos == tokens.size()) {
            error(ERROR::BRACKET, pos);
          }
        }
        var = make_shared<VariableType<string>>(name, array);
      } break;
      case TokenType::VARIABLISED_NUM:
      case TokenType::NUMBER: {
        type = TokenType::VARIABLISED_NUM;
        vector<long double> array;
        bool pass = false;
        while (pos < tokens.size()) {
          if (pos + 1 < tokens.size()) {
            if (tokens[pos + 1].type == TokenType::L_RBACKET) {
              pass = true;
            }
          } else if (tokens[pos].type == TokenType::R_SQBACKET) {
            pos++;
            break;
          }
          array.push_back(
              doMath<long double>(pos, tokens.size() - 1));
          if (pass) break;
          pos++;
        }
        if (!pass) pos--;
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        while (pass && tokens[pos].type != TokenType::R_RBACKET) {
          pos++;
          if (pos == tokens.size()) {
            error(ERROR::BRACKET, pos);
          }
        }
        var = make_shared<VariableType<long double>>(name, array);
      } break;
      case TokenType::VARIABLISED_BOOL: {
        type = TokenType::VARIABLISED_BOOL;
        shared_ptr<VariableType<bool>> _var =
            dynamic_pointer_cast<VariableType<bool>>(
                variable[stoi(tokens[pos].value)]);
        var = make_shared<VariableType<bool>>(name, _var->getvector());
      } break;
      case TokenType::FALSE:
      case TokenType::TRUE: {
        type = TokenType::VARIABLISED_BOOL;
        vector<bool> array;
        for (int i = 0; i < size; i++) {
          switch (tokens[pos + i].type) {
            case TokenType::TRUE:
              array.push_back(true);
              break;
            case TokenType::FALSE:
              array.push_back(false);
              break;
            default:
              error(ERROR::OTHER, pos + i);
              break;
          }
          if (pos + i >= tokens.size() ||
              tokens[pos + i].type == TokenType::R_SQBACKET) {
            break;
          }
        }
        while (array.size() < size) {
          array.push_back(array[array.size() - 1]);
        }
        var = make_shared<VariableType<bool>>(name, array);
      } break;
      case TokenType::FUNCTIONISED:
        if (tokens[pos + 1].type == TokenType::L_RBACKET) {
          if (tokens[pos + 2].type == TokenType::R_RBACKET) {
            runFunction(stoi(tokens[pos].value), pos);
          } else {
            int start_pos = pos++;
            vector<shared_ptr<Variable>> vars;
            while (tokens[pos].type != TokenType::R_RBACKET) {
              switch (tokens[pos].type) {
                case TokenType::VARIABLISED_BOOL: {
                  VariableType<bool> var;
                  var = *(dynamic_pointer_cast<VariableType<bool>>(
                      variable[stoi(tokens[pos].value)]));
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
                  var.newvalue(doMath<long double>(pos, pos));
                  vars.push_back(make_shared<VariableType<long double>>(var));
                } break;
                case TokenType::TRUESTRING:
                case TokenType::VARIABLISED_STR: {
                  VariableType<string> var;
                  var.newvalue(doMath<string>(pos, pos));
                  vars.push_back(make_shared<VariableType<string>>(var));
                } break;
              }
              pos++;
            }
            runFunction(stoi(tokens[start_pos].value), start_pos, vars);
            pos--;
          }

        } else {
          error(ERROR::BRACKET, pos + 1);
        }
        return defVar(_pos, end_pos);
    }
  } else if (!pass.empty()) {
    if (tokens[pos].type == TokenType::LET) {
      if (shared_ptr<VariableType<long double>> num =
              dynamic_pointer_cast<VariableType<long double>>(pass[0])) {
        type = TokenType::VARIABLISED_NUM;
        vector<long double> arr = num->getvector();
        var = make_shared<VariableType<long double>>(
            tokens[pos + 1].value, arr);
      } else if (shared_ptr<VariableType<bool>> boo =
                     dynamic_pointer_cast<VariableType<bool>>(pass[0])) {
        type = TokenType::VARIABLISED_BOOL;
        vector<bool> arr = boo->getvector();
        var = make_shared<VariableType<bool>>(tokens[pos + 1].value, arr);
      } else if (shared_ptr<VariableType<string>> str =
                     dynamic_pointer_cast<VariableType<string>>(pass[0])) {
        type = TokenType::VARIABLISED_STR;
        vector<string> arr = str->getvector();
        var = make_shared<VariableType<string>>(tokens[pos + 1].value, arr);
      }
    }
  } else {
    error(ERROR::INIT_VAR, pos);
  }
    for (int j = pos; j < end_pos; j++) {
      if (tokens[j].type == TokenType::STRING) {
        if (j > 0) {
          if (tokens[j - 1].type != TokenType::LET &&
              var->getname() == tokens[j].value) {
            tokens[j].type = type;
            tokens[j].value = to_string(variable.size());
          }
        } else {
          if (var->getname() == tokens[j].value) {
            tokens[j].type = type;
            tokens[j].value = to_string(variable.size());
          }
        }
      }
    }
  variable.push_back(var);
  return variable.size() - 1;
};

template <typename T>
T Parser_main::doMath(int &pos, int end_pos) {
  T var;
  int _pos_ = pos;
  vector<int> changedPos;
  vector<string> changedIndex;
  vector<int> changedPos_str;
  vector<string> changedIndex_str;
  vector<int> changedPos_fun;
  vector<string> changedIndex_fun;
    for (int j = pos; j <= end_pos; j++) {
      T localVar;
      if (tokens[j].type == TokenType::COMMA) break;
      _pos_ = j;
      switch (tokens[j].type) {
        case TokenType::CIN: {
          coutConfig(j, false);
            string str;
            pos = j;
            str = Consule::input();
          if constexpr (is_same_v<T, string>) {return str;}
          else if constexpr (is_same_v<T, long double>) {return stold(str);}
        }
        case TokenType::FUNCTIONISED:
          changedPos_fun.push_back(j);
          changedIndex_fun.push_back(tokens[j].value);
          if (tokens[j + 1].type == TokenType::L_RBACKET) {
            if (tokens[j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[j].value), j);
            } else {
              int start = j++;
              vector<shared_ptr<Variable>> vars;
              while (tokens[j].type != TokenType::R_RBACKET) {
                // cerr << "AT: " << i << ";" << j << endl;
                switch (tokens[j].type) {
                  case TokenType::VARIABLISED_BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[j].value)]));
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
                    var.pushValue(doMath<long double>(j, j));
                    vars.push_back(make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.pushValue(doMath<string>(j, j));
                    vars.push_back(make_shared<VariableType<string>>(var));
                  } break;
                }
                j++;
              }
              runFunction(stoi(tokens[start].value), start, vars);
              j = start;
              j--;
            }
          } else {
            error(ERROR::BRACKET, j + 1);
          }
          continue;
        case TokenType::VARIABLISED_NUM: {
          changedPos.push_back(j);
          changedIndex.push_back(tokens[j].value);
          tokens[j].type = TokenType::NUMBER;
          int index = 0;
          int _j = j;
          shared_ptr<VariableType<long double>> _var =
              dynamic_pointer_cast<VariableType<long double>>(
                  variable[stoi(tokens[j].value)]);
          if (_var->getSize() > 1) {
            _j++;
            if (tokens[j].type == TokenType::L_SQBACKET) {
              _j++;
              int startPos = _j;
              while (tokens[_j].type != TokenType::R_SQBACKET) {
                _j++;
                if (_j >= tokens.size()) {
                  error(ERROR::BRACKET, j);
                }
              }
              index = doMath<int>(startPos, _j);
            } else {
              error(ERROR::ARRAY_ADDING, j);
            }
          }
          tokens[j].value = to_string(_var->getvalue(index));
          j--;
        }
          continue;
        case TokenType::VARIABLISED_STR: {
          changedPos_str.push_back(j);
          changedIndex_str.push_back(tokens[j].value);
          tokens[j].type = TokenType::TRUESTRING;
          int index = 0;
          int _j = j;
          shared_ptr<VariableType<string>> _var =
              dynamic_pointer_cast<VariableType<string>>(
                  variable[stoi(tokens[j].value)]);
          if (_var->getSize() > 1) {
            _j++;
            if (tokens[j].type == TokenType::L_SQBACKET) {
              _j++;
              int startPos = _j;
              while (tokens[_j].type != TokenType::R_SQBACKET) {
                _j++;
                if (_j >= tokens.size()) {
                  error(ERROR::BRACKET, j);
                }
              }
              index = doMath<int>(startPos, _j);
            } else {
              error(ERROR::ARRAY_ADDING, j);
            }
          }
          tokens[j].value = _var->getvalue(index);
          j--;
        }
          continue;
        case TokenType::NUMBER:
          if (j + 1 < tokens.size()) {
            if (tokens[j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[j].type != TokenType::R_RBACKET) {
                j++;
              }
              j++;
            }
          }
          continue;
      }
    }
  if constexpr (is_same_v<T, long double>) {
    var = mathOP(pos, end_pos);
  } else if constexpr (is_same_v<T, string>) {
      for (int j = 0; j < tokens.size(); j++) {
        switch (tokens[j].type)
        {
        case TokenType::TRUESTRING:
          var += tokens[j].value;
          break;
        case TokenType::PLUS:
          continue;
        }
      }
  } else {
    error(ERROR::OTHER, pos);
  }
  #pragma omp parallel for
  for (int i = 0; i < changedIndex_fun.size(); i++) {
    tokens[changedPos_fun[i]].type = TokenType::FUNCTIONISED;
    tokens[changedPos_fun[i]].value = changedIndex_fun[i];
  }
  #pragma omp parallel for
  for (int i = 0; i < changedIndex_str.size(); i++) {
    tokens[changedPos_str[i]].type =
        TokenType::VARIABLISED_STR;
    tokens[changedPos_str[i]].value = changedIndex_str[i];
  }
  #pragma omp parallel for
  for (int i = 0; i < changedIndex.size(); i++) {
    tokens[changedPos[i]].type = TokenType::VARIABLISED_NUM;
    tokens[changedPos[i]].value = changedIndex[i];
  }
  pos = _pos_;
  return var;
}

bool Parser_main::boolOP(int &pos, int end_pos) {
  tokens_copy.erase(tokens_copy.begin(), tokens_copy.end());
  vector<int> changedLine, changedPos;
  vector<string> changedIndex;
  tokens_copy.emplace_back(TokenType::L_RBACKET, "(");
    for (int j = pos; j <= end_pos; j++) {
      switch (tokens[j].type) {
        case TokenType::VARIABLISED_BOOL: {
          changedPos.push_back(j);
          changedIndex.push_back(tokens[j].value);
          shared_ptr<VariableType<bool>> var =
              dynamic_pointer_cast<VariableType<bool>>(
                  variable[stoi(tokens[j].value)]);
          if (var->get_value(0)) {
            tokens_copy.emplace_back(TokenType::TRUE, "true");
          } else {
            tokens_copy.emplace_back(TokenType::FALSE, "false");
          }
        }
          continue;
        case TokenType::VARIABLISED_NUM: {
          tokens_copy.emplace_back(TokenType::NUMBER,
                                   to_string(doMath<long double>(j, j)));
        }
          continue;
        case TokenType::VARIABLISED_STR: {
          VariableType<string> var;
          tokens_copy.emplace_back(TokenType::TRUESTRING,
                                   doMath<string>(j, j));
        }
          continue;
        case TokenType::FUNCTIONISED: {
          if (tokens[j + 1].type == TokenType::L_RBACKET) {
            if (tokens[j + 2].type == TokenType::R_RBACKET) {
              runFunction(stoi(tokens[j].value), j);
            } else {
              int start = j++;
              vector<shared_ptr<Variable>> vars;
              while (tokens[j].type != TokenType::R_RBACKET) {
                switch (tokens[j].type) {
                  case TokenType::VARIABLISED_BOOL: {
                    VariableType<bool> var;
                    var = *(dynamic_pointer_cast<VariableType<bool>>(
                        variable[stoi(tokens[j].value)]));
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
                    var.newvalue(doMath<long double>(j, j));
                    vars.push_back(make_shared<VariableType<long double>>(var));
                  } break;
                  case TokenType::TRUESTRING:
                  case TokenType::VARIABLISED_STR: {
                    VariableType<string> var;
                    var.newvalue(doMath<string>(j, j));
                    vars.push_back(make_shared<VariableType<string>>(var));
                  } break;
                }
                j++;
              }
              runFunction(stoi(tokens[start].value), start, vars);
              j--;
            }
          } else {
            error(ERROR::BRACKET, j + 1);
          }
        }
          continue;
        case TokenType::NUMBER:
          tokens_copy.push_back(tokens[j]);
          if (j + 1 < tokens.size()) {
            if (tokens[j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[j].type != TokenType::R_RBACKET) {
                j++;
              }
              j++;
            }
          }
          continue;
        default: {
          tokens_copy.push_back(tokens[j]);
        }
          continue;
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
        for (int i = (int)brackets[j].first + 1;
             i < (int)brackets[j].second - 1; i++) {
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
    tokens[changedPos[i]].type = TokenType::FUNCTIONISED;
    tokens[changedPos[i]].value = changedIndex[i];
  }
  pos = end_pos;
  if (tokens_copy[0].type == TokenType::TRUE) {
    return true;
  }
  return false;
}

long double Parser_main::mathOP(int &pos, int end_pos) {
  // cerr << line << ":" << pos << ";" << end_line << ":" << end_pos << endl;
  math_tokens_copy.erase(math_tokens_copy.begin(), math_tokens_copy.end());
  bool stopping = false;
  math_tokens_copy.emplace_back(TokenType::L_RBACKET, "(");
    for (int j = pos; j <= end_pos; j++) {
      // cerr << i << ":" << j << endl;
      switch (tokens[j].type) {
        case TokenType::COMMA:
        case TokenType::COLON:
          stopping = true;
          break;
        case TokenType::NUMBER:
          math_tokens_copy.push_back(tokens[j]);
          if (j + 1 < tokens.size()) {
            if (tokens[j + 1].type == TokenType::L_RBACKET) {
              j++;
              while (tokens[j].type != TokenType::R_RBACKET) {
                j++;
              }
              j++;
            }
          }
          continue;
        default:
          math_tokens_copy.push_back(tokens[j]);
          continue;
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
        for (int i = (int)math_brackets[j].first + 1;
             i < (int)math_brackets[j].second - 1; i++) {   
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
          for (int i = (int)math_brackets[j].first + 1;
               i < (int)math_brackets[j].second - 1; i++) {
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
    math_scanFunc();
  }
  pos = end_pos;
  return stold(math_tokens_copy[0].value);
}