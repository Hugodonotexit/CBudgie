#pragma once
#include <vector>

#include "Token.h"

class AST : private Token {
 private:
  Token token;
  std::vector<AST*> children;

 public:
    AST(TokenType type, std::string code) {
        token = {type,code};
    };
    
    ~AST() {
        for (size_t i = 0; i < children.size(); i++)
        {
            if (children[i] != nullptr) {
                delete children[i];
            }
        }
    };

    void addChild(AST* child) {
        children.push_back(child);
    };

    std::vector<AST*>& getChildren() {return children;};

    TokenType& getType() {return token.tokenType;};

    std::string& getValue(){return token.code;};
};
