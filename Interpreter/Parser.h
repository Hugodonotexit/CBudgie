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
    std::vector<Token>::iterator token;

    void parsing(std::vector<std::vector<Token>>& tokenized_code, AST* bytecode);

    AST* parseClass();
    AST* parseClassVarDec();
    AST* parseSubroutine();
    AST* parseParameterList();
    AST* parseSubroutineBody();
    AST* parseVarDec();

    AST* parseStatements();
    AST* parseLet();
    AST* parseIf();
    AST* parseWhile();
    AST* parseDo();
    AST* parseReturn();

    AST* parseExpression();
    AST* parseTerm();

    bool next();
    Token* current();
    bool have(TokenType expectedType, std::string expectedValue);
    bool have(TokenType expectedType);
    Token* mustBe(TokenType expectedType, std::string expectedValue);
public:
    Parser(std::filesystem::path file);
    Parser(std::filesystem::path file, std::filesystem::path destination);
};