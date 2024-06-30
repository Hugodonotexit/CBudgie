#include <string>

#ifndef VAR_H
#define VAR_H

using namespace std;

enum class TokenType {
    /*INT8,INT16,INT32,INT64,
    FLOAT4,FLOAT8,FLOAT16,
    UINT8,UINT16,UINT32,UINT64,
    STRINGTYPE,BOOL,*/
    ASINH,ACOSH,ATANH,ASIN,ACOS,ATAN,SINH,COSH,TANH,SIN,COS,TAN,
   
    FILE,

    IF, ELSE,

    //SWITCH, CASE, DEFAULT,

    FOR, WHILE,

    BREAK, CONTINUE, RETURN,

    DEF, LET, AS, TRUE, FALSE,

    TRUESTRING, STRING, NUMBER,
    POW, ABS, LN,
    PLUS, MINUS,
    TIMES, DIVIDE,
    MODE, EQUAL,
    //INC, DEC,

    GREATER, SMALLER,
    E_GREATER, E_SMALLER,
    AND, OR,
    NOT, XOR,
    EQUIVALENT,
    NOT_EQUIVALENT,

    //ADDRESS,
    COMMA, COLON,

    L_RBACKET, R_RBACKET,
    L_SQBACKET, R_SQBACKET,
    L_SBACKET, R_SBACKET, 

    CIN, COUT,
    
    VARIABLISED_BOOL, VARIABLISED_NUM, VARIABLISED_STR, FUNCTIONISED,

    NA
};

struct Token {
    TokenType type;
    string value;
    
    Token(TokenType type, const string value) : type(type), value(value) {}
};

enum class ERROR {
    BRACKET, INIT_VAR, ARRAY_ADDING, UNDEF, OTHER
};

#endif