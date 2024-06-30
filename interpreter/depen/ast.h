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
  Operation(){};
   long double mod(long double &a, long double &b) { return std::fmod(a,b); }
   long double add(long double &a, long double &b) { return a + b; }
   long double minus(long double &a, long double &b) { return a - b; }
   long double times(long double &a, long double &b) { return a * b; }
   long double divide(long double &a, long double &b) {
    if (b == 0) {
      throw logic_error("ERROR_DIVIDED_BY_ZERO");
      return nan("");
    }
    return a / b;
  }
   long double pow(long double &a, long double &b) { return std::pow(a, b); }
   long double negative(long double &a) { return -a; }
   long double abs(long double &a) { return std::fabs(a); }
   long double ln(long double &a) { return std::log(a); }
   long double sin(long double &a) { return std::sin(a); }
   long double asin(long double &a) { return std::asin(a); }
   long double sinh(long double &a) { return std::sinh(a); }
   long double asinh(long double &a) { return std::asinh(a); }
   long double cos(long double &a) { return std::cos(a); }
   long double acos(long double &a) { return std::acos(a); }
   long double cosh(long double &a) { return std::cosh(a); }
   long double acosh(long double &a) { return std::acosh(a); }
   long double tan(long double &a) { return std::tan(a); }
   long double atan(long double &a) { return std::atan(a); }
   long double tanh(long double &a) { return std::tanh(a); }
   long double atanh(long double &a) { return std::atanh(a); }
};

class Variable {
 private:
  string name;

 public:
  Variable() {}
  Variable(string newName) : name(newName) {}
  void setname(string newName) { name = newName; }
  string getname() { return name; }
  virtual ~Variable() {}
};

template <typename T>
class VariableType : public Variable {
 private:
  vector<T> value;
  long double maxValue;
  long double step;
 public:
  // Constructor to create a vector of given size and initialize with val
  VariableType() {}
  VariableType(string newName, vector<T> val) : Variable(newName), value(val) {}

  // Set the value at a specific index
  void setvalue(size_t index, T &newvalue) {
    if (index < value.size()) {
      value[index] = newvalue;
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  void newMaxValue(long double newvalue) {
    maxValue = newvalue;
  }

  void newStep(long double newstep) {
    step = newstep;
  }

  void newvalue(T newvalue) {
    value.clear();
    value.push_back(newvalue);
  }

  void pushValue(T newvalue) {
    value.push_back(newvalue);
  }

  // Get the value at a specific index
  T &getvalue(int index) {
    if (index < value.size()) {
      return value[index];
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  long double getMaxValue() {return maxValue;}

  long double getStep() {return step;}

    bool get_value(int index) {
    if (index < value.size()) {
      return value[index];
    } else {
      throw out_of_range("Index out of bounds");
    }
  }
  

  vector<T> &getvector() {
    return value;
  }

  int getSize() { return value.size(); }

  // Destructor to clean up allocated memory
  ~VariableType() {}
};

class Scope {
 public:
  struct Location {
    int temp = -1;
    int line;
    int pos;
    Location() : line(temp), pos(temp) {}
  };

 private:
  Location start;
  Location end;
  vector<shared_ptr<Variable>> var;

 public:
  Scope() {}
  void setStartPos(int line, int pos) {
    start.line = line;
    start.pos = pos;
  }
  void setEndtPos(int line, int pos) {
    end.line = line;
    end.pos = pos;
  }
  Location getStartPos() { return start; }
  Location getEndPos() { return end; }
  void pushBackVar(shared_ptr<Variable> a) { var.push_back(a); }
  virtual void setConStartPos(int line, int pos) {}
  virtual void setConEndPos(int line, int pos) {}
  virtual Location getConStartPos() {}
  virtual Location getConEndPos() {}
  bool inScope(int i, int j) {
    if (i > start.line && i < end.line) {
      return true;
    } else if (i == start.line || i == end.line) {
      if (j > start.pos && j < end.pos) {
        return true;
      } else {
        return false;
      }
    }
    return false;
  }
  virtual bool inConScope(int i, int j) { return false; }
  virtual ~Scope() {}
};

class File : public Scope {
 private:
  string &file_name;

 public:
  File(string &newName) : file_name(newName) {}
  void setname(string &newName) { file_name = newName; }
  string &getname() { return file_name; }
};

class If : public Scope {
 private:
  Location condition_start;
  Location condition_end;

 public:
  If() {}
  void setConStartPos(int line, int pos) {
    condition_start.line = line;
    condition_start.pos = pos;
  }
  void setConEndPos(int line, int pos) {
    condition_end.line = line;
    condition_end.pos = pos;
  }
  Location getConStartPos() {return condition_start;}
  Location getConEndPos() {return condition_end;}
  bool inConScope(int i, int j) {
    if (i > condition_start.line && i < condition_end.line) {
      return true;
    } else if (i == condition_start.line || i == condition_end.line) {
      if (j > condition_start.pos && j < condition_end.pos) {
        return true;
      } else {
        return false;
      }
    }
    return false;
  }
};

class Function : public If {
 private:
  string &name;

 public:
  Function(string &newName) : name(newName) {}
  void setname(string &newName) { name = newName; }
  string &getname() { return name; }
};

class Else : public Scope {
 public:
  Else() {}
};

class While : public If {
 private:
 public:
  While() {}
};

class For : public If {
 private:
 public:
  For() {}
};

class Switch : public If {
 public:
  Switch() {}
};

class Consule {
 public:
  Consule() {}
  inline static void output(string out) { cout << out; }
  inline static string input() {
    string in;
    cin >> in;
    return in;
  }
  ~Consule() {}
};

#endif