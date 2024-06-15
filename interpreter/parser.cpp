#include "parser.h"

Parser::Parser(vector<vector<Token>> &token) : tokens(&token) {
  for (int line = 0; line < tokens->size(); line++) {
    for (int pos = 0; pos < tokens[line].size(); pos++) {
      switch (tokens->at(line).at(pos).type) {
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
        default:
          break;
      }
    }
  }
}

void Parser::defFile(int &line, int &pos) {
    File file(tokens->at(line).at(pos + 1).value);
    int i = line;
    int j = pos;
    do
    {
        if (j == tokens->at(i).size()) {
            j = 0;
            i++;
        }
    } while (!(tokens->at(i).at(j).value == "`"));
    file.setEndtPos(i,j);
}

template <typename T>
void Parser::defScope(int &line, int &pos) {
    if constexpr (std::is_same_v<T, Function>) {
    if (tokens->at(line).at(pos + 1).type == TokenType::STRING) {
      for (int i = 0; i < scope.size(); i++) {
        if (std::dynamic_pointer_cast<T>(scope[i])) {
          if (tokens->at(line).at(pos + 1).value == std::dynamic_pointer_cast<T>(scope[i])->getname()) {
            throw std::runtime_error("Redefining at " + to_string(line) + ":" + to_string(pos));
            return;
          }
        }
      }
    } else {
        throw std::runtime_error("Unknow Error at " + to_string(line) + ":" + to_string(pos));
        return;
    }
    }
   

  int openbracket = 0;
  int closebracket = 0;
  int i = line;
  int j = pos;
  
  std::optional<T> scoOpt;

  if constexpr (std::is_same_v<T, For>)
  {
    scoOpt.emplace(tokens->at(line).at(pos + 3).value, tokens->at(line).at(pos + 5).value);
  } else {
    scoOpt.emplace(tokens->at(line).at(pos + 1).value);
  }
  
  T& sco = scoOpt.value();
  
  sco.setStartPos(line, pos);
  do {
    switch (tokens->at(i).at(j).type) {
      case TokenType::L_SBACKET:
        openbracket++;
      case TokenType::R_SBACKET:
        closebracket++;
        break;
      default:
        break;
    }
    if (j == tokens->at(i).size() - 1) {
      j = 0;
      i++;
    }
  } while (openbracket == closebracket && openbracket != 0);
  sco.setEndtPos(i, j);
  scope.push_back(std::make_shared<T>(sco));
};

Parser::~Parser() { delete (tokens); }