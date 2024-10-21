#pragma once
#include <filesystem>
#include <vector>
#include <fstream>
#include "../dependance/AST.h"
#include "../dependance/Opcode.h"
#include "Interpreter.h"
#include <unordered_map>
#include <future>
#include <stack>
#include <algorithm>
class Lexer
{
private:
    std::vector<std::string> lines;

    std::unordered_map<std::string, TokenType> keywords = {
        {"def", TokenType::DEF}, {"read", TokenType::READ}, {"print", TokenType::PRINT},
        {"if", TokenType::IF}, {"else", TokenType::ELSE}, {"for", TokenType::FOR},
        {"while", TokenType::WHILE}, {"true", TokenType::TRUE},
        {"false", TokenType::FALSE}, {"break", TokenType::BREAK}, {"continue", TokenType::CONTINUE},
        {"toSting", TokenType::TO_STRING}, {"toBool", TokenType::TO_BOOL}, {"toNum", TokenType::TO_NUM},
        {"numeric", TokenType::NUMERIC}, {"return", TokenType::RETURN}, {"import", TokenType::IMPORT}
    };

    std::vector<std::string> openfile(const std::filesystem::path& filePath);
    std::vector<Token> preprocessLine(std::string line);
    Token readIdentifierOrKeyword(const std::string& line, int& i);
    Token readNumber(const std::string& line, int& i);
    bool isIdentifierStart(char c, char next);
    bool isIdentifierPart(char c);
    bool isNumberStart(char current,char next);
public:
    Lexer(std::vector<std::string>& lines);
    std::vector<std::vector<Token>> run();
};
