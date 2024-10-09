#pragma once
#include <string>
enum class TokenType {
    DEF,

    READ,
    PRINT,

    IF,
    ELSE,

    WHILE,
    FOR,

    NUM_CONST,
    TRUE,
    FALSE,
    WORD_CONST,

    VARIABLE,
    
    BREAK,
    CONTINUE,
    RETURN,

    PLUS,
    MINUS,
    TIMES,
    DIVIDE,
    MODE,
    POW,
    EQUAL,

    GREATER,
    SMALLER,
    E_GREATER,
    E_SMALLER,
    AND,
    OR,
    NOT,
    XOR,
    EQUIVALENT,
    NOT_EQUIVALENT,
    
    NUM,

    COMMA,
    COLON,
    LINKER,

    L_RBRACKET,
    R_RBRACKET,
    L_SQBRACKET,
    R_SQBRACKET,
    L_SBRACKET,
    R_SBRACKET,

    TO_STRING,
    TO_BOOL,
    TO_NUM,
};

struct Token
{
    TokenType tokenType;
    std::string code;

    Token(TokenType tokenType, std::string code) : tokenType(tokenType), code(code) {}
    Token() {}
};
