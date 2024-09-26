#pragma once
#include <filesystem>
#include <vector>
#include <fstream>
#include "../dependance/Token.h"
#include "../dependance/Opcode.h"
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <stack>
#include <algorithm>
class Lexer
{
private:
    std::unordered_map<std::string, TokenType> keywords = {
        {"def", TokenType::DEF}, {"read", TokenType::READ}, {"print", TokenType::PRINT},
        {"if", TokenType::IF}, {"else", TokenType::ELSE}, {"for", TokenType::FOR},
        {"while", TokenType::WHILE}, {"true", TokenType::TRUE},
        {"false", TokenType::FALSE}, {"break", TokenType::BREAK}, {"continue", TokenType::CONTINUE},
        {"toSting", TokenType::TO_STRING}, {"toBool", TokenType::TO_BOOL}, {"toNum", TokenType::TO_NUM},
        {"return", TokenType::RETURN}
    };
    std::filesystem::path filePath;
    std::vector<std::vector<Token>>& tokenized_code;
    std::mutex& mutex_;
    std::condition_variable& cv_;
    bool& done_;

    void preprocessLine(std::vector<Token>& tokenized_line, std::vector<int>& math_op, int equal_op, int swap_op, int return_swap_op);
    Token readIdentifierOrKeyword(const std::string& line, int& i);
    Token readNumber(const std::string& line, int& i);
    bool isIdentifierStart(char c);
    bool isIdentifierPart(char c);
    bool isLeftAssociative(TokenType type);
    int getPrecedence(TokenType type);
    std::vector<Token> reorderExpression(const std::vector<Token>& vec);
public:
    Lexer(const std::filesystem::path& filePath, std::vector<std::vector<Token>>& tokenized_code, std::mutex& mtx, std::condition_variable& cv, bool& done);
    void run();
};
