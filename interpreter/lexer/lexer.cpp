#include <cmath>
#include "lexer.h"

#include "var.h"

using namespace std;

Lexer::~Lexer() {}

Lexer::Lexer(const vector<string>& input) : input(input) {
  tokenPatterns = {
      {TokenType::ASINH, "asinh"},{TokenType::ACOSH, "acosh"},{TokenType::ATANH, "atanh"},{TokenType::ASIN, "asin"},{TokenType::ACOS, "acos"},{TokenType::ATAN, "atan"},
      {TokenType::SINH, "sinh"},{TokenType::COSH, "cosh"},{TokenType::TANH, "tanh"},{TokenType::SIN, "sin"},{TokenType::COS, "cos"},{TokenType::TAN, "tan"},

      {TokenType::ABS, "abs"}, {TokenType::LN, "ln"},

      //{TokenType::INC, "++"},
      //{TokenType::DEC, "--"},
      {TokenType::EQUIVALENT, "=="},
      {TokenType::XOR, "!|"},
      {TokenType::E_GREATER, ">="},
      {TokenType::E_SMALLER, "<="},  //"long OP with conflict symbol"

      //{TokenType::BOOL, "bool"},

      {TokenType::FILE, "`"},

      {TokenType::IF, "if"},
      {TokenType::ELSE, "else"},

      {TokenType::SWITCH, "switch"},
      {TokenType::CASE, "case"},
      {TokenType::DEFAULT, "default"},

      {TokenType::FOR, "for"},
      {TokenType::WHILE, "while"},

      {TokenType::BREAK, "break"},
      {TokenType::CONTINUE, "continue"},
      {TokenType::RETURN, "return"},

      {TokenType::DEF, "def"},
      {TokenType::LET, "let"},
      //{TokenType::AS, "as"},
      {TokenType::TRUE, "True"},
      {TokenType::FALSE, "False"},

      //{TokenType::STRING, "\""}, //{TokenType::NUMBER, "."}, // Special case
      {TokenType::POW, "^"},
      {TokenType::PLUS, "+"},
      {TokenType::MINUS, "-"},
      {TokenType::TIMES, "*"},
      {TokenType::DIVIDE, "/"},
      {TokenType::MODE, "%"},
      {TokenType::EQUAL, "="},

      {TokenType::GREATER, ">"},
      {TokenType::SMALLER, "<"},

      {TokenType::AND, "&"},
      {TokenType::OR, "|"},
      {TokenType::NOT, "!"},

      //{TokenType::ADDRESS, "@"},
      {TokenType::COMMA, ","},

      {TokenType::L_RBACKET, "("},
      {TokenType::R_RBACKET, ")"},
      {TokenType::L_SQBACKET, "["},
      {TokenType::R_SQBACKET, "]"},
      {TokenType::L_SBACKET, "{"},
      {TokenType::R_SBACKET, "}"},

      {TokenType::CIN, "cin"},
      {TokenType::COUT, "cout"},
  };
}

bool Lexer::isChar(char a) {
  if (isalpha(a) || isdigit(a) || a == '_') {
    return true;
  }
  return false;
}

vector<vector<Token>> Lexer::tokenize() {
  vector<vector<Token>> tokens;
  for (int line = 0; line < input.size(); line++) {
    int pos = 0;
    vector<Token> tokens_line;
    while (pos < input[line].size()) {
      if (isspace(input[line][pos])) {
        pos++;
        continue;
      } else if (input[line][pos] == '#') {
        break;
      }

      bool isFound = false;
      #pragma omp parallel for
      for (auto& pattern : tokenPatterns) {
        if (input[line].substr(pos, pattern.second.size()) == pattern.second) {
          tokens_line.emplace_back(
              pattern.first, input[line].substr(pos, pattern.second.size()));
          pos += pattern.second.size();
          isFound = true;
          break;
        }
      }
      if (isFound) {
        continue;
      }

      if (input[line][pos] == '\"') {
        bool isFound = false;
        int i = pos;
        do {
          i++;
          if (input[line][i] == '\"') {
            isFound = true;
          } else if (i == input[line].size()) {
            throw runtime_error("Missing a closing \" at" + to_string(line) + ":" + to_string(pos));
          }

        } while (!isFound);
        string str = input[line].substr(pos+1, i - pos - 1);
        int  x = 0;
        tokens_line.emplace_back(TokenType::TRUESTRING, str);
        pos = i + 1;
        continue;
      }

      if (isdigit(input[line][pos]) || input[line][pos] == '.') {
        int dot = 0;
        if (input[line][pos] == '.') {
          dot++;
        }

        bool isNum = true;
        int start = pos;
        int _pos_ = pos + 1;

        while (isNum) {
          if (isdigit(input[line][_pos_]) || input[line][_pos_] == '.') {
            if (input[line][pos] == '.') {
              dot++;
            }
            if (dot > 1) {
              throw runtime_error("Multiple Dot at" + to_string(line) + ":" +
                                  to_string(_pos_));
            }
            _pos_++;
            if (_pos_ == input[line].size()) {
              break;
            }
            continue;
          } else {
            isNum = false;
          }
        }
        int length = _pos_ - start;
        tokens_line.emplace_back(TokenType::NUMBER, input[line].substr(start, length));
        pos += length;
        continue;
      }

      int i = pos;
      do {
        i++;
        if (i == input[line].size()) {
          break;
        }
      } while (isChar(input[line][i]));
      tokens_line.emplace_back(TokenType::STRING,input[line].substr(pos, i - pos));
      pos = i;
    }
    if (!tokens_line.empty())
    {
      tokens.push_back(tokens_line);
    } 
  }

  return tokens;
}
