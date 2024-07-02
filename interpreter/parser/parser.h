#ifndef PARSER_H
#define PARSER_H
#include <omp.h>

#include <memory>
#include <optional>
#include <sstream>
#include <vector>

#include "../depen/ast.h"
#include "parser_math.h"

class Parser_main : public MathSupport {
 private:
  template <typename T>
  inline int defScope(int pos) {
    int openbracket = 0;
    int closebracket = 0;
    int openRoundBracket = 0;
    int closeRoundBracket = 0;
    int j = pos;

    optional<T> scoOpt;

    if constexpr (is_same_v<T, Function>) {
      scoOpt.emplace(tokens[pos + 1].value);
    } else {
      scoOpt.emplace();
    }

    T &sco = scoOpt.value();

    while (openbracket != closebracket || openbracket == 0 || closebracket == 0) {
      j++;
      if (j >= tokens.size()) {
        error(ERROR::BRACKET, j);
      }

      switch (tokens[j].type) {
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
        sco.setConStartPos(j);
        openRoundBracket++;
      } else if (closeRoundBracket == 1) {
        sco.setConEndPos(j);
        closeRoundBracket++;
      } else if (openbracket == 1) {
        sco.setStartPos(j);
        openbracket++;
        closebracket++;
      }
    }
    sco.setEndtPos(j);
    if constexpr (is_same_v<T, Function>) {
      for (int i = 0; i < tokens.size(); i++) {
          if (tokens[i].type == TokenType::STRING) {
            if (tokens[i-1].type != TokenType::DEF && sco.getname() == tokens[i].value) {
              tokens[i].type = TokenType::FUNCTIONISED;
              tokens[i].value = to_string(scope.size());
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
  inline void defFile(int pos) {
    File file(tokens[pos + 1].value);
    int i = pos;
    do {
      i++;
      if (i >= tokens.size()) error(ERROR::BRACKET,i);
    } while (!(tokens[i].value == "`"));
    file.setEndtPos(i);
    scope.push_back(make_shared<File>(file));
  }
  inline int defVar(int &pos, int end_pos) {
    vector<shared_ptr<Variable>> pass;
    return defVar(pos, end_pos, pass);
  }
  int defVar(int &pos, int end_pos, vector<shared_ptr<Variable>> pass);

  inline bool runFunction(int index) { return runFunction(index, -1); }

  inline bool runFunction(int index, int _pos_) {
    vector<shared_ptr<Variable>> var;
    return runFunction(index, -1, var);
  }
  inline bool runFunction(int index, vector<shared_ptr<Variable>> var) {
    return runFunction(index, -1, var);
  }
  bool runFunction(int index, int __pos__, vector<shared_ptr<Variable>> var);

  template <typename T>
  T doMath(int &pos, int end_pos);
  bool boolOP(int &pos, int end_pos);
  long double mathOP(int &pos, int end_pos);

 public:
  Parser_main(vector<Token> tokens, map<int,int> &range);
  ~Parser_main();
};

#endif