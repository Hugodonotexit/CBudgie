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
    std::filesystem::path filePath;
    std::vector<std::vector<Token>> tokenized_code;
    size_t lineIndex;
    size_t tokenIndex;

    AST* parseVariableDeclaration();

    AST* parseClass();
    AST* parseFunction();
    AST* parseMethod();
    AST* parseParameterList();
    AST* parseClassSubroutineBody();
    AST* parseSubroutineBody();

    AST* parseStatements();
    AST* parseIf();
    AST* parseFor();
    AST* parseWhile();
    AST* parseDo();
    AST* parseReturn();
    
    AST* parseCondition();
    AST* parseForCondition();

    AST* parseExpression();
    AST* parseAssignment();
    AST* parseEqualityExpression();
    AST* parseAdditiveExpression();
    AST* parseMultiplicativeExpression();
    AST* parseUnaryExpression();
    AST* parseExponentiationExpression();
    AST* parsePrimaryExpression();
    AST* parseLogicalOrExpression();
    AST* parseLogicalXorExpression();
    AST* parseLogicalAndExpression();
    AST* parseRelationalExpression();
    AST* parseArgumentList();

    AST* parseBuiltInFunctionCall();
    AST* parseDot(AST* node);

    bool next();
    Token* current();
    Token* lookahead(int offset);
    bool have(TokenType expectedType, const std::string& expectedValue);
    bool have(TokenType expectedType);
    Token* mustBe(TokenType expectedType, const std::string& expectedValue);
    Token* mustBe(TokenType expectedType);

    bool isAtEndOfLine();
public:
    std::string tokenTypeToString(TokenType type);
    void printAST(AST* node, int indent = 0);

    Parser(std::vector<std::vector<Token>>& tokenized_code);
    AST* run();
};