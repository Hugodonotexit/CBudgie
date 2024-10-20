#pragma once
#include <filesystem>
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "Lexer.h"
#include "../dependance/Token.h"
#include "../dependance/Opcode.h"
#include "../dependance/AST.h"
#include "../dependance/Error.h"
#include "Translator.h"
class Parser
{
private:
    int index_token;
    std::filesystem::path filePath;
    std::vector<std::vector<Token>> tokenized_code;
    int lineIndex;
    int tokenIndex;

    AST* parseClass();
    AST* parseFunction();
    AST* parseParameterList();
    AST* parseClassSubroutineBody();
    AST* parseSubroutineBody();

    AST* parseStatements();
    AST* parseIf();
    AST* parseWhile();
    AST* parseDo();
    AST* parseReturn();
    
    AST* parseCondition();

    AST* parseExpression();
    AST* parseAssignment();
    AST* parseEqualityExpression();
    AST* parseAdditiveExpression();
    AST* parseMultiplicativeExpression();
    AST* parseUnaryExpression();
    AST* parsePrimaryExpression();
    AST* parseLogicalOrExpression();
    AST* parseLogicalAndExpression();
    AST* parseRelationalExpression();
    AST* parseArgumentList();

    bool next();
    Token* current();
    bool have(TokenType expectedType, const std::string& expectedValue);
    bool have(TokenType expectedType);
    Token* mustBe(TokenType expectedType, const std::string& expectedValue);
    Token* mustBe(TokenType expectedType);

    bool isAtEndOfLine();
public:
    Parser(std::vector<std::vector<Token>>& tokenized_code);
    AST* run();
};