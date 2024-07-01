#ifndef PARSER_H
#define PARSER_H
#include <omp.h>

#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <vector>

#include "../depen/ast.h"
#include "parser_math.h"

class Parser_main : public MathSupport {
 private:
  template <typename T>
  inline int defScope(int line, int pos) {
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

    while (openbracket != closebracket || openbracket == 0 ||
           closebracket == 0) {
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
      for (int i = 0; i < tokens.size(); i++) {
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
  inline void defFile(int line, int pos) {
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
  inline int defVar(int &line, int &pos, int end_line, int end_pos) {
    vector<shared_ptr<Variable>> pass;
    return defVar(line, pos, end_line, end_pos, pass);
  }
  int defVar(int &line, int &pos, int end_line, int end_pos,
             vector<shared_ptr<Variable>> pass);

  inline bool runFunction(int index) { return runFunction(index, -1, -1); }

  inline bool runFunction(int index, int _line_, int _pos_) {
    vector<shared_ptr<Variable>> var;
    return runFunction(index, -1, -1, var);
  }
  inline bool runFunction(int index, vector<shared_ptr<Variable>> var) {
    return runFunction(index, -1, -1, var);
  }
  bool runFunction(int index, int __line__, int __pos__,
                   vector<shared_ptr<Variable>> var);

  template <typename T>
  T doMath(int &line, int &pos, int end_line, int end_pos);
  bool boolOP(int &line, int &pos, int end_line, int end_pos);
  long double mathOP(int &line, int &pos, int end_line, int end_pos);

 public:
  Parser_main(vector<vector<Token>> tokens);
  ~Parser_main();
};

#endif