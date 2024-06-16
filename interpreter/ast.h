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
protected:
    struct Location
    {
        int temp = -1;
        int &line;
        int &pos;
        Location() : line(temp), pos(temp) {}
    };
private:
    Location start;
    Location end;
public:
    Scope() {}
    void setStartPos(int &line, int &pos) {
        start.line = line;
        start.pos = pos;
        }
    void setEndtPos(int &line, int &pos) {
        end.line = line;
        end.pos = pos;
        }
    virtual void setConStartPos(int &line, int &pos) {}
    virtual void setConEndPos(int &line, int &pos) {}
    virtual ~Scope() {}
};

class File : public Scope
{
private:
    std::string &file_name;
public:
    File(std::string &newName): file_name(newName) {}
    void setname(std::string &newName) {file_name = newName;}
    std::string& getname() {return file_name;}
};

class If : public Scope
{
private:
    Location condition_start;
    Location condition_end;
public:
    If() {}
    void setConStartPos(int &line, int &pos) {
        condition_start.line = line;
        condition_start.pos = pos;
        }
    void setConEndPos(int &line, int &pos) {
        condition_end.line = line;
        condition_end.pos = pos;
        }
};

class Function : public If
{
private:
    std::string &name;
public:
    Function(std::string &newName): name(newName) {}
    void setname(std::string &newName) {name = newName;}
    std::string& getname() {return name;}
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
    While() {}
};

class For : public If
{
private:

public:
    For() {}
};

class Switch : public If
{
public:
    Switch() {}
};

template<typename T>
class Variable : public Scope
{
private:
    std::string &name;
    T value;
public:
    Variable(std::string &newName, T value): name(newName), value(value) {}
    void setname(std::string &newName) {name = newName;}
    std::string& getname() {return name;}
};

#endif