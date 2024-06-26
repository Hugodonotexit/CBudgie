#include "parser.h"
Parser_error::Parser_error() {}
Parser_error::~Parser_error() {}

void Parser_error::error(ERROR type, int line, int pos) {
  switch (type) {
    case ERROR::BRACKET:
      throw logic_error("Missing bracket at " + to_string(line) + ":" +
                        to_string(pos));
      break;
    case ERROR::INIT_VAR:
      throw logic_error("Varaible not initialised at " + to_string(line) + ":" +
                        to_string(pos));
      break;
    case ERROR::ARRAY_ADDING:
      throw logic_error("adding array not supported at " + to_string(line) +
                        ":" + to_string(pos));
      break;
    case ERROR::OTHER:
      throw logic_error("unexpected error at " + to_string(line) + ":" +
                        to_string(pos));
      break;
    case ERROR::UNDEF:
      throw invalid_argument("undefined Varaible/Function error at " +
                             to_string(line) + ":" + to_string(pos));
      break;
  }
}