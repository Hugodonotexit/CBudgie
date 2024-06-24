/*
MIT License

Copyright (c) 2024 Hugodonotexit

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

1.Attribution:
  All users of this Software must give appropriate credit to the original
creators. Such credit shall include, but not be limited to, the inclusion of the
following elements in any derived works, distributions, or publications: -The
name and version of the Software used. -A link to the original Software or the
original creator’s website (if applicable). -A clear statement that the work is
derived from or built with the Software. -The attribution must be placed in a
location that is reasonably conspicuous, ensuring that anyone using or
interacting with the derivative work is aware of the original creators'
contribution. 2.Commercial Use: Use of the Software for commercial purposes is
subject to prior approval by the original creators. Entities or individuals
wishing to use the Software for commercial gain must first seek and obtain
written permission from the copyright holders. The request for permission must
include: -A detailed description of the intended commercial use. -The expected
duration of such use. -Any potential modifications or derivatives to be created
from the Software. -Approval for commercial use is at the sole discretion of the
copyright holders and may be subject to specific terms and conditions as
stipulated at the time of approval. 3.Enforcement: Failure to comply with these
conditions will be considered a breach of the license and may result in
revocation of the rights granted under this License. The copyright holders
reserve the right to take any action deemed necessary to enforce compliance with
these terms, including legal proceedings.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef SGTFUNCTION_H
#define SGTFUNCTION_H
#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <utility>
#include <variant>
#include <future>
#include <vector>

#include "../depen/ast.h"

// The Func class is designed to parse, validate, and evaluate mathematical
// functions represented as strings. It extends the sgt::Math class and utilizes
// various helper methods to process the function.

class Func : protected Operation {
 private:
  // Vector arrays to hold operators based on their precedence
  std::vector<int> operators[3];

  // The mathematical function represented as a string
  std::string _function;

  // Variant type to handle different types of function elements (double, char,
  // string)
  using varsFunc = std::variant<long double, char, std::string>;

  // Vector to store the parsed function elements
  std::vector<varsFunc> function;

  // Vector to store pairs of bracket indices
  std::vector<std::pair<int, int>> brackets;

  // Method to validate the function
  void checkFunc();

  // Method to scan and process the function
  void scanFunc();

  // Method to clean 'NA' elements from the function
  void cleanNAN();

  // Method to clean redundant brackets
  void cleanBracket();

  // Method to locate brackets in the function
  void locateBrackets();

  // Method to locate operators in the function
  void locateOps();

  // Method to validate long operators like "sin", "cos", etc.
  bool isValidLongOps(std::string s);

  // Method to push the function elements into the vector
  void pushFunc();

  // Method to validate characters in the function
  bool isValidCharacter(char c);

  // Method to check if a value is an integer
  bool isInteger(long double value);

  // Method to calculate the result for a given element
  void calculate(varsFunc &element);

 public:
  // Default constructor
  Func();

  // Constructor that initializes the function with a given string
  Func(std::string _function);

  // Method to set the function
  void setFunc(std::string _function);

  // Method to get the function
  std::string getFunc() const;

  // Method to print the parsed function !!!DEBUGGING ONLY!!!
  void printCompiled() const;

  // Method to get the y-value for a given x-value
  long double get_y();

  // Destructor
  ~Func();
};

inline bool Func::isInteger(long double value) { return std::floor(value) == value; }

inline bool Func::isValidCharacter(char c) {
  static const std::unordered_set<char> validChars = {
      '.', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
      '+', '-', '*', '/', '^', 'a', 'h', 'i',
      'l', 'n', 'o', 's', 't', 'c', '(', ')'};
  return validChars.find(c) != validChars.end();
}

inline bool Func::isValidLongOps(std::string s) {
  static const std::unordered_set<std::string> validLongOps = {
      "ln",   "sin",   "asin", "sinh", "asinh", "cos",  "acos",
      "cosh", "acosh", "tan",  "atan", "tanh",  "atanh"};
  return validLongOps.find(s) != validLongOps.end();
}

inline Func::Func(){};
inline Func::Func(std::string _function) { setFunc(_function); }
inline void Func::setFunc(std::string _function) {
  this->_function.erase(this->_function.begin(), this->_function.end());
  this->function.erase(this->function.begin(), this->function.end());

  this->_function = _function;
  try {
    checkFunc();
  } catch (const std::logic_error &e) {
    std::cerr << "Undefined function!!! " << e.what() << std::endl;
    _function = "NULL";
    function.erase(function.begin(), function.end());
  }
};
inline Func::~Func(){};
inline std::string Func::getFunc() const { return _function; };
inline void Func::printCompiled() const {
  for (int i = 0; i < (int)function.size(); i++) {
    if (std::holds_alternative<long double>(function[i])) {
      std::cout << std::get<long double>(function[i]);
    } else if (std::holds_alternative<char>(function[i])) {
      std::cout << std::get<char>(function[i]);
    } else if (std::holds_alternative<std::string>(function[i])) {
      std::cout << std::get<std::string>(function[i]);
    } else {
      throw std::logic_error("ERROR05");
      return;
    }
  }
  std::cout << std::endl;
}

inline void Func::checkFunc() {
  if (_function[0] == '/' || _function[0] == '*' || _function[0] == '^') {
    throw std::logic_error("ERROR00a");
    return;
  }
  if (_function[_function.size() - 1] == '(' ||
      _function[_function.size() - 1] == '/' ||
      _function[_function.size() - 1] == '*' ||
      _function[_function.size() - 1] == '+' ||
      _function[_function.size() - 1] == '^' ||
      _function[_function.size() - 1] == '-') {
    throw std::logic_error("ERROR00b");
    return;
  }

  for (int i = 0; i < (int)_function.size(); i++) {
    if (_function.size() > 1 && _function[i] == '(' &&
        (_function[i + 1] == '/' || _function[i + 1] == '*' ||
         _function[i + 1] == '^')) {
      throw std::logic_error("ERROR00c");
      return;
    }
    if (!isValidCharacter(_function[i])) {
      throw std::logic_error("ERROR00d");
      return;
    }
  }
};

inline void Func::scanFunc() {
  cleanBracket();
  cleanNAN();
  locateBrackets();
  locateOps();
}
inline void Func::cleanNAN() {
  for (int i = 0; i < (int)function.size(); i++) {
    if (std::holds_alternative<std::string>(function[i])) {
      if (std::get<std::string>(function[i]) == "NA") {
        function.erase(function.begin() + i);
        i--;
      }
    }
  }
};

inline void Func::cleanBracket() {
  for (int i = 0; i < (int)function.size(); i++) {
    if (std::holds_alternative<char>(function[i]) &&
        std::holds_alternative<char>(function[i + 2]) &&
        std::holds_alternative<long double>(function[i + 1])) {
      if (std::get<char>(function[i]) == '(' &&
          std::get<char>(function[i + 2]) == ')') {
        function[i] = "NA";
        function[i + 2] = "NA";
      }
    }
  }
}

inline void Func::locateBrackets() {
  brackets.erase(brackets.begin(), brackets.end());
  std::vector<int> openBracket, closeBracket;
  for (int i = 0; i < (int)function.size(); i++) {
    if (std::holds_alternative<char>(function[i])) {
      if (std::get<char>(function[i]) == '(') {
        openBracket.push_back(i);
      } else if (std::get<char>(function[i]) == ')') {
        closeBracket.push_back(i);
      }
    }
  }
  if (openBracket.size() != closeBracket.size()) {
    throw std::logic_error("ERROR01a");
    return;
  }
  for (int i = (int)openBracket.size() - 1; i >= 0; i--) {
    if (openBracket[i] >= closeBracket[i]) {
      throw std::logic_error("ERROR01b");
      return;
    }
  }
  for (int i = 0; i < (int)openBracket.size(); i++) {
    for (int j = closeBracket.size(); j >= 0; j--) {
      brackets.push_back(std::make_pair(openBracket[i], closeBracket[j]));
    }
  }
}

inline void Func::locateOps() {
  operators[0].erase(operators[0].begin(), operators[0].end());
  operators[1].erase(operators[1].begin(), operators[1].end());
  operators[2].erase(operators[2].begin(), operators[2].end());
  for (int i = 0; i < (int)function.size(); i++) {
    if (std::holds_alternative<char>(function[i])) {
      if (std::get<char>(function[i]) == '^') {
        operators[0].push_back(i);
      } else if (std::get<char>(function[i]) == '*' ||
                 std::get<char>(function[i]) == '/') {
        operators[1].push_back(i);
      } else if (std::get<char>(function[i]) == '+' ||
                 std::get<char>(function[i]) == '-') {
        operators[2].push_back(i);
      }

    } else if (std::holds_alternative<std::string>(function[i])) {
      if (isValidLongOps(std::get<std::string>(function[i]))) {
        operators[1].push_back(i);
      }
    }
  }
}

inline void Func::pushFunc() {
  function.erase(function.begin(), function.end());

  bool isNum = false;
  int numCharStart = 0;

  function.push_back((char)'(');

  for (int i = 0; i < (int)_function.size(); i++) {
    if ((std::isdigit(_function[i]) || _function[i] == '.') && !isNum) {
      isNum = true;
      numCharStart = i;
    } else if (!(std::isdigit(_function[i]) || _function[i] == '.') && isNum) {
      isNum = false;
      long double num = std::stod(_function.substr(numCharStart, i - numCharStart));
      function.push_back(num);
      i--;
    } else if ((std::isdigit(_function[i]) || _function[i] == '.') && isNum) {
    } else if (_function.substr(i, 2) == "ln") {
      function.push_back("ln");
      i++;
    } else if (_function.substr(i, 5) == "asinh") {
      function.push_back("asinh");
      i += 4;
    } else if (_function.substr(i, 5) == "acosh") {
      function.push_back("acosh");
      i += 4;
    } else if (_function.substr(i, 5) == "atanh") {
      function.push_back("atanh");
      i += 4;
    } else if (_function.substr(i, 4) == "asin") {
      function.push_back("asin");
      i += 3;
    } else if (_function.substr(i, 4) == "acos") {
      function.push_back("acos");
      i += 3;
    } else if (_function.substr(i, 4) == "atan") {
      function.push_back("atan");
      i += 3;
    } else if (_function.substr(i, 4) == "sinh") {
      function.push_back("sinh");
      i += 3;
    } else if (_function.substr(i, 4) == "cosh") {
      function.push_back("cosh");
      i += 3;
    } else if (_function.substr(i, 4) == "tanh") {
      function.push_back("tanh");
      i += 3;
    } else if (_function.substr(i, 3) == "sin") {
      function.push_back("sin");
      i += 2;
    } else if (_function.substr(i, 3) == "cos") {
      function.push_back("cos");
      i += 2;
    } else if (_function.substr(i, 3) == "tan") {
      function.push_back("tan");
      i += 2;
    } else {
      function.push_back(_function[i]);
    }
  }

  if (isNum) {
    long double num = std::stod(_function.substr(
        numCharStart, _function.size() - (size_t)numCharStart));
    function.push_back(num);
  }

  function.push_back((char)')');
};

inline void Func::calculate(varsFunc &element) {
  varsFunc *func = &element;
  if (std::holds_alternative<char>(*func) &&
      std::holds_alternative<char>(*(func - 1)) &&
      std::holds_alternative<long double>(*(func + 1))) {
    if (std::get<char>(*(func - 1)) == '(') {
      switch (std::get<char>(*func)) {
        case '-':
          *func = "NA";
          *(func + 1) = negative(std::get<long double>(*(func + 1)));
          break;

        case '+':
          *func = "NA";
          break;
      }
    }

  } else if (std::holds_alternative<char>(*func) &&
             std::holds_alternative<long double>(*(func - 1)) &&
             std::holds_alternative<long double>(*(func + 1))) {
    switch (std::get<char>(*func)) {
      case '^':
        *func =
            pow(std::get<long double>(*(func - 1)), std::get<long double>(*(func + 1)));
        *(func + 1) = "NA";
        *(func - 1) = "NA";
        break;
      case '*':
        *func =
            times(std::get<long double>(*(func - 1)), std::get<long double>(*(func + 1)));
        *(func + 1) = "NA";
        *(func - 1) = "NA";
        break;
      case '/':
        *func = divide(std::get<long double>(*(func - 1)),
                       std::get<long double>(*(func + 1)));
        *(func + 1) = "NA";
        *(func - 1) = "NA";
        break;
      case '+':
        *func =
            add(std::get<long double>(*(func - 1)), std::get<long double>(*(func + 1)));
        *(func + 1) = "NA";
        *(func - 1) = "NA";
        break;
      case '-':
        *func =
            minus(std::get<long double>(*(func - 1)), std::get<long double>(*(func + 1)));
        *(func + 1) = "NA";
        *(func - 1) = "NA";
        break;
      default:
        throw std::logic_error("ERROR03a");
        return;
        break;
    }
  } else if (std::holds_alternative<std::string>(*func) &&
             std::holds_alternative<long double>(*(func + 1))) {
    if (std::get<std::string>(*func) == "ln") {
      *(func + 1) = ln((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "sin")) {
      *(func + 1) = sin((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "cos")) {
      *(func + 1) = cos((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "tan")) {
      *(func + 1) = tan((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "asin")) {
      *(func + 1) = asin((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "acos")) {
      *(func + 1) = acos((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "atan")) {
      *(func + 1) = atan((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "sinh")) {
      *(func + 1) = sinh((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "cosh")) {
      *(func + 1) = cosh((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "tanh")) {
      *(func + 1) = tanh((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "asinh")) {
      *(func + 1) = asinh((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "acosh")) {
      *(func + 1) = acosh((std::get<long double>(*(func + 1))));
    } else if ((std::get<std::string>(*func) == "atanh")) {
      *(func + 1) = atanh((std::get<long double>(*(func + 1))));
    }
    *func = "NA";
  } else {
    throw std::logic_error("ERROR03d");
    return;
  }
}


inline long double Func::get_y() {
  pushFunc();
  int runtime = 0;
  int initSize = function.size();
  do {
    if (runtime > initSize) {
      throw std::runtime_error(
          "ERROR: YOU MAY DISCOVER A BUG, REPORT IT ON "
          "https://github.com/Hugodonotexit/libgraph/issues !!!");
      return std::nan("");
    }

    scanFunc();
    if (!brackets.empty()) {
      for (int j = brackets.size() - 1; j >= 0; j--) {
        for (int i = (int)brackets[j].first + 1;
             i < (int)brackets[j].second - 1; i++) {
          for (int k = 0; k < (int)operators[0].size(); k++) {
            if (i == operators[0][k]) {
              calculate(function[i]);
              scanFunc();
            }
          }
        }

        bool foundInOperators0 = false;
        for (int i = (int)brackets[j].first + 1;
             i < (int)brackets[j].second - 1; i++) {
          for (int k = 0; k < (int)operators[0].size(); k++) {
            if (i == operators[0][k]) {
              foundInOperators0 = true;
              break;
            }
          }
          if (foundInOperators0) {break;}
        }

        if (!foundInOperators0) {
          for (int i = (int)brackets[j].first + 1;
               i < (int)brackets[j].second - 1; i++) {
            for (int k = 0; k < (int)operators[1].size(); k++) {
              if (i == operators[1][k]) {
                calculate(function[i]);
                scanFunc();
              }
            }
          }
          bool foundInOperators1 = false;
          for (int i = (int)brackets[j].first + 1;
               i < (int)brackets[j].second - 1; i++) {
            for (int k = 0; k < (int)operators[1].size(); k++) {
              if (i == operators[1][k]) {
                foundInOperators1 = true;
                break;
              }
            }
            if (foundInOperators1) {break;}
          }

          if (!foundInOperators1) {
            for (int i = (int)brackets[j].first + 1;
                 i < (int)brackets[j].second - 1; i++) {
              for (int k = 0; k < (int)operators[2].size(); k++) {
                if (i == operators[2][k]) {
                  calculate(function[i]);
                  scanFunc();
                }
              }
            }
          }
        }
      }
    }
    cleanNAN();
    runtime++;
  } while (function.size() > 1);

  return std::get<long double>(function[0]);
}
#endif