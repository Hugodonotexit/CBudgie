#ifndef AST_H
#define AST_H
#include <cmath>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#include "var.h"

using namespace std;

class Operation {
 protected:
  inline Operation(){};
  inline long double mod(long double &a, long double &b) { return std::fmod(a, b); }
  inline long double add(long double &a, long double &b) { return a + b; }
  inline long double minus(long double &a, long double &b) { return a - b; }
  inline long double times(long double &a, long double &b) { return a * b; }
  inline long double divide(long double &a, long double &b) {
    if (b == 0) {
      throw logic_error("ERROR_DIVIDED_BY_ZERO");
      return nan("");
    }
    return a / b;
  }
  inline long double pow(long double &a, long double &b) { return std::pow(a, b); }
  inline long double negative(long double &a) { return -a; }
  inline long double abs(long double &a) { return std::fabs(a); }
  inline long double ln(long double &a) { return std::log(a); }
  inline long double sin(long double &a) { return std::sin(a); }
  inline long double asin(long double &a) { return std::asin(a); }
  inline long double sinh(long double &a) { return std::sinh(a); }
  inline long double asinh(long double &a) { return std::asinh(a); }
  inline long double cos(long double &a) { return std::cos(a); }
  inline long double acos(long double &a) { return std::acos(a); }
  inline long double cosh(long double &a) { return std::cosh(a); }
  inline long double acosh(long double &a) { return std::acosh(a); }
  inline long double tan(long double &a) { return std::tan(a); }
  inline long double atan(long double &a) { return std::atan(a); }
  inline long double tanh(long double &a) { return std::tanh(a); }
  inline long double atanh(long double &a) { return std::atanh(a); }
};

class Variable {
 private:
  string name;

 public:
  inline Variable() {}
  inline Variable(string newName) : name(newName) {}
  inline void setname(string newName) { name = newName; }
  inline string getname() { return name; }
  inline virtual ~Variable() {}
};

template <typename T>
class VariableType : public Variable {
 private:
  vector<T> value;
  long double maxValue;
  long double step;

 public:
  // Constructor to create a vector of given size and initialize with val
  inline VariableType() {}
  inline VariableType(string newName, vector<T> val) : Variable(newName), value(val) {}

  // Set the value at a specific index
  inline void setvalue(size_t index, T &newvalue) {
    if (index < value.size()) {
      value[index] = newvalue;
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  inline void newMaxValue(long double newvalue) { maxValue = newvalue; }

  inline void newStep(long double newstep) { step = newstep; }

  inline void newvalue(T newvalue) {
    value.clear();
    value.push_back(newvalue);
  }

 inline void pushValue(T newvalue) { value.push_back(newvalue); }

  // Get the value at a specific index
  inline T &getvalue(int index) {
    if (index < value.size()) {
      return value[index];
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  inline long double getMaxValue() { return maxValue; }

  inline long double getStep() { return step; }

  inline bool get_value(int index) {
    if (index < value.size()) {
      return value[index];
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  inline vector<T> &getvector() { return value; }

  inline int getSize() { return value.size(); }

  // Destructor to clean up allocated memory
  ~VariableType() {}
};

class Scope {
  private:
  int start;
  int end;
  vector<shared_ptr<Variable>> var;

 public:
  Scope() {}
  inline void setStartPos(int pos) {
    start = pos;
  }
  inline void setEndtPos(int pos) {
    end = pos;
  }
  inline int getStartPos() { return start; }
  inline int getEndPos() { return end; }
  inline void pushBackVar(shared_ptr<Variable> a) { var.push_back(a); }
  inline virtual void setConStartPos(int pos) {}
  inline virtual void setConEndPos(int pos) {}
  inline virtual int getConStartPos() {return start;}
  inline virtual int getConEndPos() {return end;}
  inline virtual ~Scope() {}
};

class File : public Scope {
 private:
  string &file_name;

 public:
  inline File(string &newName) : file_name(newName) {}
  inline void setname(string &newName) { file_name = newName; }
  inline string &getname() { return file_name; }
};

class If : public Scope {
 private:
  int condition_start;
  int condition_end;
 public:
  inline If() {}
  inline void setConStartPos(int pos) {
    condition_start = pos;
  }
  inline void setConEndPos(int pos) {
    condition_end = pos;
  }
  inline int getConStartPos() { return condition_start; }
  inline int getConEndPos() { return condition_end; }
};

class Else : public Scope {
 public:
  inline Else() {}
};

class Function : public If {
 private:
  string &name;

 public:
  inline Function(string &newName) : name(newName) {}
  inline void setname(string &newName) { name = newName; }
  inline string &getname() { return name; }
};

class While : public If {
 private:
 public:
  inline While() {}
};

class For : public If {
 private:
 public:
  inline For() {}
};

class Switch : public If {
 public:
  inline Switch() {}
};

class Consule {
 public:
  inline Consule() {}
  inline static void output(string out) { cout << out; }
  inline static string input() {
    string in;
    cin >> in;
    return in;
  }
  inline ~Consule() {}
};

#endif