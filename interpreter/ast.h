#ifndef AST_H
#define AST_H
#include <vector>
#include <cmath>
#include "var.h"

using namespace std;

template<typename T>
class Operation
{
public:
    Operation() {};
    void add(T& a, T& b, T& answer) {answer = a + b;}
    void minus(T& a, T& b, T& answer) {answer = a - b;}
    void times(T& a, T& b, T& answer) {answer = a * b;}
    void divide(T& a, T& b, T& answer) {answer = a / b;}
    void mod(T& a, T& b, T& answer) {answer = mod(a,b);}
};

class Scope
{
private:
    int stratLine, startPos, endLine, endPos;
public:
    Scope() {}
    void setStartPos(int line, int pos) {
        stratLine = line;
        startPos = pos;
        }
    void setEndtPos(int line, int pos) {
        endLine = line;
        endPos = pos;
        }
    virtual ~Scope() {}
};

class Function : public Scope
{
private:
    std::string &name;
public:
    Function(std::string &newName): name(newName) {}
    void setname(std::string &newName) {name = newName;}
    std::string& getname() {return name;}
};

class If : public Scope
{
private:
    std::string &condition;
public:
    If(std::string &condition) : condition(condition) {}
};

class Else : public Scope
{
public:
    Else() {}
};

class While : public If
{
private:
    
public:
    While(std::string &condition) : If(condition) {}
};

class For : public If
{
private:
    std::string &rule;
public:
    For(std::string &condition, std::string &rule) : rule(rule), If(condition) {}
};

class Switch : public If
{
public:
    Switch(std::string &condition) : If(condition) {}
};

#endif