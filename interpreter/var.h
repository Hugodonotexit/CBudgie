#include <string>

#ifndef VAR_H
#define VAR_H

enum class TokenType {
    INT8,INT16,INT32,INT64,
    FLOAT4,FLOAT8,FLOAT16,
    UINT8,UINT16,UINT32,UINT64,
    CHAR,STRINGTYPE,WCHAR,BOOL,

    FILE,

    IF, ELSE,

    SWITCH, CASE, DEFAULT,

    FOR, WHILE,

    BREAK, CONTINUE, RETURN,

    DEF, LET, AS,

    TRUESTRING, STRING, NUMBER,
    PLUS, MINUS,
    TIMES, DIVIDE,
    MODE, EQUAL,
    INC, DEC,

    GREATER, SMALLER,
    E_GREATER, E_SMALLER,
    AND, OR,
    NOT, XOR,
    EQUIVALENT,

    ADDRESS, COMMA,

    L_RBACKET, R_RBACKET,
    L_SQBACKET, R_SQBACKET,
    L_SBACKET, R_SBACKET, 

    CIN, COUT
};

struct Token {
    TokenType type;
    std::string value;
    
    Token(TokenType type, const std::string value) : type(type), value(value) {}
};

#endif