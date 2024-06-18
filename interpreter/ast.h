#ifndef AST_H
#define AST_H
#include <vector>
#include <cmath>
#include <memory>
#include <stdexcept>
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

class Variable
{
private:
    std::string &name;
public:
    Variable(std::string &newName): name(newName) {}
    void setname(std::string &newName) {name = newName;}
    std::string& getname() {return name;}
    virtual ~Variable() {}
};

template<typename T>
class VariableType : public Variable
{
private:
    std::vector<T*> *value;
public:
    // Constructor to create a vector of given size and initialize with val
    VariableType(std::string &newName, size_t newSize, std::vector<T*> val) : Variable(newName) {
        value = new std::vector<T*>(val); // Copy the vector of pointers
    }

    // Set the value at a specific index
    void setvalue(size_t index, T &newvalue) {
        if (index < value->size()) {
            *(*value)[index] = newvalue;
        } else {
            throw out_of_range("Index out of bounds");
        }
    }

    void newvalue(T &newvalue) {
        value->push_back(new T(newvalue));
    }

    // Get the value at a specific index
    T& getvalue(size_t index) const {
        if (index < value->size()) {
            return *(*value)[index];
        } else {
            throw out_of_range("Index out of bounds");
        }
    }

    // Destructor to clean up allocated memory
    ~VariableType() {
        for (size_t i = 0; i < value->size(); ++i) {
            delete (*value)[i];
        }
        delete value;
    }
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
    vector<shared_ptr<Variable>> var;
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
    void pushBackVar(shared_ptr<Variable> a) {
        var.push_back(a);
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

#endif