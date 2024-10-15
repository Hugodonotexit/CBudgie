#pragma once
#include <vector>

#include "Token.h"

class AST {
 private:
  TokenType tokenType;
  std::string code;
  std::vector<AST*> children;

 public:
  AST(TokenType type, std::string code) : tokenType(type), code(code) {};

  ~AST() {
    for (size_t i = 0; i < children.size(); i++) {
      if (children[i] != nullptr) {
        delete children[i];
      }
    }
  };

  void addChild(AST* child) { children.push_back(child); };

  const std::vector<AST*>& getChildren() const { return children; };

  const TokenType& getType() const { return tokenType; };

  const std::string& getValue() const { return code; };
};

class Token : public AST {
 public:
  Token(TokenType tokenType, std::string code) : AST(tokenType, code) {}
};