#ifndef PARSER_ERROR_H
#define PARSER_ERROR_H
#include <iostream>
#include <map>

#include "../depen/ast.h"

class Parser_error {
 protected:
  map<int,int> range;
  vector<Token> tokens;
  Parser_error(vector<Token> &token,map<int,int> &range) : range(range), tokens(token) {}
  ~Parser_error() {}

  inline void error(ERROR type, int pos) {
    auto it = range.lower_bound(pos);
    cerr << "error occured at " << it->second - 1 << ":" << pos - (--it)->first + 1
         << "-->"<<  tokens[pos].value << " " << tokens[pos+1].value << " " << tokens[pos+2].value << endl;
    switch (type) {
      case ERROR::BRACKET:
        throw logic_error("Missing bracket");
        break;
      case ERROR::INIT_VAR:
        throw logic_error("Varaible not initialised");
        break;
      case ERROR::ARRAY_ADDING:
        throw logic_error("adding array not supported");
        break;
      case ERROR::OTHER:
        throw logic_error("unexpected error");
        break;
      case ERROR::UNDEF:
        throw invalid_argument("undefined Varaible/Function error");
        break;
    }
  }
};
#endif