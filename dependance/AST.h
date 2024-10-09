#pragma once
#include "Token.h"
#include <vector>

class ASTNode 
{
protected:
    Token tok;
    std::vector<ASTNode*>* children;
public:
    ASTNode() {}
    ASTNode(const Token& tok) : tok(tok) {
        children = new std::vector<ASTNode*>;
    };
    virtual std::vector<ASTNode*>* arrTok() {
        return children;
    };
    virtual ASTNode* arrTok_back() {
        return children->back();
    };
    virtual void arrTok_push_back(const Token& tok) {
        ASTNode* node = new ASTNode(tok);
        children->push_back(node);
    }
    virtual void setToken(const Token& tok) {
        this->tok=tok;
    };
    virtual Token getToken() {
        return tok;
    };
    ~ASTNode() {
        for (int i = 0; i < children->size(); i++)
        {
            delete children->at(i);
        }
         delete children;
    };
};