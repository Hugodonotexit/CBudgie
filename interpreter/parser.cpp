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
      }
    }
  }
}

std::shared_ptr<Variable> Parser::defVar(int &line, int &pos) {
  shared_ptr<Variable> var;
  std::optional<TokenType> type;
#pragma omp parallel for
  for (int i = pos; i < tokens[line].size(); i++) {
    if (tokens[line][i].type == TokenType::AS) {
      type = tokens[line][i + 1].type;
      break;
    }
  }
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

  if (!type.has_value() &&
      tokens[line][pos + 2 + step].type != TokenType::EQUAL) {
    throw runtime_error("not initialise varaible without defining type at " +
                        to_string(line) + ":" + to_string(pos));
  } else if (!type.has_value() &&
             tokens[line][pos + 2 + step].type == TokenType::EQUAL) {
    int step2 = 0;
    if (tokens[line][pos + 3 + step].type == TokenType::L_SQBACKET) {
      step2++;
    }
    switch (tokens[line][pos + 3 + step + step2].type) {
      case TokenType::TRUESTRING: {
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
        var = make_shared<VariableType<string>>(tokens[line][pos + 1].value,
                                                size, array);
      } break;
      case TokenType::NUMBER: {
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
    }
  } else if (type.has_value() &&
             tokens[line][pos + 2 + step].type == TokenType::EQUAL) {
  } else if (type.has_value() &&
             tokens[line][pos + 2 + step].type != TokenType::EQUAL) {
    switch (type.value()) {
      case TokenType::STRINGTYPE: {
        vector<string *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new std::string("NULL");
        }
        var = make_shared<VariableType<string>>(tokens[line][pos + 1].value,
                                                size, array);
      } break;
      case TokenType::BOOL: {
        vector<bool *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new bool("NULL");
        }
        var = make_shared<VariableType<bool>>(tokens[line][pos + 1].value, size,
                                              array);
      } break;
      case TokenType::UINT8: {
        vector<uint_least8_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new uint_least8_t(0);
        }
        var = make_shared<VariableType<uint_least8_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::INT8: {
        vector<int_least8_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new int_least8_t(0);
        }
        var = make_shared<VariableType<int_least8_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::UINT16: {
        vector<uint_least16_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new uint_least16_t(0);
        }
        var = make_shared<VariableType<uint_least16_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::INT16: {
        vector<int_least16_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new int_least16_t(0);
        }
        var = make_shared<VariableType<int_least16_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::UINT32: {
        vector<uint_least32_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new uint_least32_t(0);
        }
        var = make_shared<VariableType<uint_least32_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::INT32: {
        vector<int_least32_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new int_least32_t(0);
        }
        var = make_shared<VariableType<int_least32_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::UINT64: {
        vector<uint_least64_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new uint_least64_t(0);
        }
        var = make_shared<VariableType<uint_least64_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::INT64: {
        vector<int_least64_t *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new int_least64_t(0);
        }
        var = make_shared<VariableType<int_least64_t>>(
            tokens[line][pos + 1].value, size, array);
      } break;
      case TokenType::FLOAT4: {
        vector<float *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new float(0);
        }
        var = make_shared<VariableType<float>>(tokens[line][pos + 1].value,
                                               size, array);
      } break;
      case TokenType::FLOAT8: {
        vector<double *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new double(0);
        }
        var = make_shared<VariableType<double>>(tokens[line][pos + 1].value,
                                                size, array);
      } break;
      case TokenType::FLOAT16: {
        vector<long double *> array(size);
        for (size_t i = 0; i < size; ++i) {
          array[i] = new long double(0);
        }
        var = make_shared<VariableType<long double>>(
            tokens[line][pos + 1].value, size, array);
      } break;
    }
  }
  return var;
}

void Parser::defFile(int &line, int &pos) {
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
void Parser::defScope(int &line, int &pos) {
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

  do {
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
      default:
        break;
    }
    if (openRoundBracket == 1) {
      sco.setConStartPos(i, j);
    } else if (closeRoundBracket == 1) {
      sco.setConEndPos(i, j);
    } else if (openbracket == 1) {
      sco.setStartPos(i, j);
    }

    if (tokens[i][j].type == TokenType::LET) {
      sco.pushBackVar(defVar(i, j));
    }

  } while (!(openbracket == closebracket) || openbracket == 0 ||
           closebracket == 0);
  sco.setEndtPos(i, j);
  if constexpr (std::is_same_v<T, Function>) {
    if (sco.getname() == "main") {
      scope.insert(scope.begin(), std::make_shared<T>(sco));
    }

  } else {
    scope.push_back(std::make_shared<T>(sco));
  }
};

Parser::~Parser() {}