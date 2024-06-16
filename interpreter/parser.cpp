#include "parser.h"

Parser::Parser(vector<vector<Token>> &token) : tokens(&token) {
  for (int line = 0; line < tokens->size(); line++) {
    #pragma omp parallel for
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
        case TokenType::LET:
          //defVar(line, pos);
          break;
        default:
          break;
      }
    }
  }
}

void Parser::defVar(int &line, int &pos) {
    File file(tokens->at(line).at(pos + 1).value);
    int i = line;
    int j = pos;
    do
    {
      j++;
        if (j == tokens->at(i).size()) {
            if (i < tokens->size() - 1)
            {
              i++;
              j = 0;
            } else {
              j--;
              break;
            }
        }
    } while (!(tokens->at(i).at(j).value == "`"));
    file.setEndtPos(i,j);
}

void Parser::defFile(int &line, int &pos) {
    File file(tokens->at(line).at(pos + 1).value);
    int i = line;
    int j = pos;
    do
    {
      j++;
      if (j == tokens->at(i).size()) {
      i++;
      j=0;
      if (i == tokens->size())
      {
        i--;
        j = tokens->at(i).size() - 1;
        break;
      }
    }
    } while (!(tokens->at(i).at(j).value == "`"));
    file.setEndtPos(i,j);
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

  if constexpr (std::is_same_v<T, Function>)
  {
    scoOpt.emplace(tokens->at(line).at(pos + 1).value);
  } else {
    scoOpt.emplace();
  }
  
  T& sco = scoOpt.value();
  
  
  do {
    j++;
    if (j >= tokens->at(i).size()) {
      i++;
      j=0;
      if (i >= tokens->size())
      {
        throw std::runtime_error("Missing close bracket at " + to_string(i) + ":" + to_string(j));
        abort();
      }
    }
    switch (tokens->at(i).at(j).type) {
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
    if (openRoundBracket == 1)
    {
      sco.setConStartPos(i,j);
    } else if (closeRoundBracket == 1) {
      sco.setConEndPos(i,j);
    } else if (openbracket == 1) {
      sco.setStartPos(i,j);
    }
    
  } while (!(openbracket == closebracket) || openbracket == 0 || closebracket == 0);
  sco.setEndtPos(i, j);
  scope.push_back(std::make_shared<T>(sco));
};

Parser::~Parser() {}