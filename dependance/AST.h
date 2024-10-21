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

  virtual ~AST() {
    for (auto child : children) {
      if (child != nullptr) {
        delete  child;
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
  ~Token() override = default;
};