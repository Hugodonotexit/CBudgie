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
   long double pow(long double &a, long double &b) { return pow(a, b); }
   long double negative(long double &a) { return -a; }
   long double abs(long double &a) { return fabs(a); }
   long double ln(long double &a) { return log(a); }
   long double sin(long double &a) { return sin(a); }
   long double asin(long double &a) { return asin(a); }
   long double sinh(long double &a) { return sinh(a); }
   long double asinh(long double &a) { return asinh(a); }
   long double cos(long double &a) { return cos(a); }
   long double acos(long double &a) { return acos(a); }
   long double cosh(long double &a) { return cosh(a); }
   long double acosh(long double &a) { return acosh(a); }
   long double tan(long double &a) { return tan(a); }
   long double atan(long double &a) { return atan(a); }
   long double tanh(long double &a) { return tanh(a); }
   long double atanh(long double &a) { return atanh(a); }
};

class Variable {
 private:
  string name;

 public:
  Variable(string newName) : name(newName) {}
  void setname(string newName) { name = newName; }
  string getname() { return name; }
  virtual ~Variable() {}
};

template <typename T>
class VariableType : public Variable {
 private:
  vector<T *> *value;

 public:
  // Constructor to create a vector of given size and initialize with val
  VariableType(string newName, size_t newSize, vector<T *> val)
      : Variable(newName) {
    value = new vector<T *>(val);  // Copy the vector of pointers
  }

  // Set the value at a specific index
  void setvalue(size_t index, T &newvalue) {
    if (index < value->size()) {
      *(*value)[index] = newvalue;
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  void newvalue(T newvalue) {
    value->clear();
    value->push_back(new T(newvalue));
  }

  // Get the value at a specific index
  T &getvalue(int index) const {
    if (index < value->size()) {
      return *(*value)[index];
    } else {
      throw out_of_range("Index out of bounds");
    }
  }

  vector<T *> &getvector() const {
    return *value;
  }

  int getSize() { return value->size(); }

  // Destructor to clean up allocated memory
  ~VariableType() {
    for (size_t i = 0; i < value->size(); ++i) {
      delete (*value)[i];
    }
    delete value;
  }
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
  inline static void output(string out) { cout << out << endl; }
  inline static string input() {
    string in;
    cin >> in;
    return in;
  }
  ~Consule() {}
};

#endif