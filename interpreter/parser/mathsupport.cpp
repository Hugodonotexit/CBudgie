#include "parser.h"
MathSupport::MathSupport(vector<vector<Token>> &tokens) : BoolSupport(tokens) {}
MathSupport::~MathSupport() {}

void MathSupport::math_scanFunc() {
  cleanBracket();
  cleanNAN();
  locateBrackets();
  locateOps();
}

void MathSupport::cleanNAN() {
  for (int i = 0; i < (int)math_tokens_copy.size(); i++) {
    if (math_tokens_copy[i].type == TokenType::NA) {
      math_tokens_copy.erase(math_tokens_copy.begin() + i);
      i--;
    }
  }
};

void MathSupport::cleanBracket() {
  for (int i = 0; i < (int)math_tokens_copy.size() - 2; i++) {
    if (math_tokens_copy[i].type == TokenType::L_RBACKET &&
        math_tokens_copy[i + 2].type == TokenType::R_RBACKET) {
      math_tokens_copy[i].type = TokenType::NA;
      math_tokens_copy[i + 2].type = TokenType::NA;
    }
  }
}

void MathSupport::locateOps() {
  math_operators[0].erase(math_operators[0].begin(), math_operators[0].end());
  math_operators[1].erase(math_operators[1].begin(), math_operators[1].end());
  math_operators[2].erase(math_operators[2].begin(), math_operators[2].end());
  for (int i = 0; i < (int)math_tokens_copy.size(); i++) {
    switch (math_tokens_copy[i].type) {
      case TokenType::ASINH:
      case TokenType::ACOSH:
      case TokenType::SINH:
      case TokenType::COSH:
      case TokenType::ABS:
      case TokenType::LN:
      case TokenType::ATANH:
      case TokenType::TANH:
      case TokenType::ASIN:
      case TokenType::SIN:
      case TokenType::ACOS:
      case TokenType::COS:
      case TokenType::ATAN:
      case TokenType::TAN:
        math_operators[0].push_back(i);
        break;
      case TokenType::TIMES:
      case TokenType::DIVIDE:
      case TokenType::MODE:
      case TokenType::POW:
        math_operators[1].push_back(i);
        break;
      case TokenType::PLUS:
      case TokenType::MINUS:
        math_operators[2].push_back(i);
        break;
    }
  }
}

void MathSupport::locateBrackets() {
  math_brackets.erase(math_brackets.begin(), math_brackets.end());
  std::vector<int> openBracket, closeBracket;
  for (int i = 0; i < (int)math_tokens_copy.size(); i++) {
    if (math_tokens_copy[i].type == TokenType::L_RBACKET) {
      openBracket.push_back(i);
    } else if (math_tokens_copy[i].type == TokenType::R_RBACKET) {
      closeBracket.push_back(i);
    }
  }
  if (openBracket.size() != closeBracket.size()) {
    error(ERROR::BRACKET, -1, -1);
    return;
  }
  for (int i = (int)openBracket.size() - 1; i >= 0; i--) {
    if (openBracket[i] >= closeBracket[i]) {
      error(ERROR::OTHER, -1, -2);
      return;
    }
  }
  for (int i = 0; i < (int)openBracket.size(); i++) {
    for (int j = closeBracket.size() - 1; j >= 0; j--) {
      math_brackets.emplace_back(openBracket[i], closeBracket[j]);
    }
  }
}

void MathSupport::math_calculate(int index) {
  switch (math_tokens_copy[index].type) {
    case TokenType::ASINH: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(asinh(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::ACOSH: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(acosh(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::SINH: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(sinh(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::COSH: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(cosh(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::ABS: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(abs(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::LN: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(ln(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::ATANH: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(atanh(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::TANH: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(tanh(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::ASIN: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(asin(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::SIN: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(sin(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::ACOS: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(acos(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::COS: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(cos(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::ATAN: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(atan(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::TAN: {
      if (math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(tan(a));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::TIMES: {
      if (math_tokens_copy[index - 1].type != TokenType::NUMBER ||
          math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index - 1].value);
      long double b = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(times(a, b));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index - 1].type = TokenType::NA;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::DIVIDE: {
      if (math_tokens_copy[index - 1].type != TokenType::NUMBER ||
          math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index - 1].value);
      long double b = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(divide(a, b));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index - 1].type = TokenType::NA;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::MODE: {
      if (math_tokens_copy[index - 1].type != TokenType::NUMBER ||
          math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index - 1].value);
      long double b = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(mod(a, b));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index - 1].type = TokenType::NA;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::POW: {
      if (math_tokens_copy[index - 1].type != TokenType::NUMBER ||
          math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index - 1].value);
      long double b = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(pow(a, b));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index - 1].type = TokenType::NA;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::PLUS: {
      if (math_tokens_copy[index - 1].type != TokenType::NUMBER ||
          math_tokens_copy[index + 1].type != TokenType::NUMBER) {
        error(ERROR::OTHER, -1, -1);
      }
      long double a = stold(math_tokens_copy[index - 1].value);
      long double b = stold(math_tokens_copy[index + 1].value);
      math_tokens_copy[index].value = to_string(add(a, b));
      math_tokens_copy[index].type = TokenType::NUMBER;
      math_tokens_copy[index - 1].type = TokenType::NA;
      math_tokens_copy[index + 1].type = TokenType::NA;
    } break;
    case TokenType::MINUS: {
      if (math_tokens_copy[index - 1].type != TokenType::NUMBER ||
          math_tokens_copy[index + 1].type == TokenType::NUMBER) {
        long double a = stold(math_tokens_copy[index + 1].value);
        math_tokens_copy[index].value = to_string(negative(a));
        math_tokens_copy[index].type = TokenType::NUMBER;
        math_tokens_copy[index + 1].type = TokenType::NA;
      } else if (math_tokens_copy[index - 1].type == TokenType::NUMBER &&
                 math_tokens_copy[index + 1].type == TokenType::NUMBER) {
        long double a = stold(math_tokens_copy[index - 1].value);
        long double b = stold(math_tokens_copy[index + 1].value);
        math_tokens_copy[index].value = to_string(minus(a, b));
        math_tokens_copy[index].type = TokenType::NUMBER;
        math_tokens_copy[index - 1].type = TokenType::NA;
        math_tokens_copy[index + 1].type = TokenType::NA;
      } else {
        error(ERROR::OTHER, -1, -1);
      }
    } break;
  }
}
