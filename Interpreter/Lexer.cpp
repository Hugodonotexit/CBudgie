#include "Lexer.h"

#include <iostream>

Lexer::Lexer(const std::filesystem::path& filePath,
             std::vector<std::vector<Token>>& tokenized_code, std::mutex& mtx,
             std::condition_variable& cv, bool& done)
    : filePath(filePath),
      tokenized_code(tokenized_code),
      mutex_(mtx),
      cv_(cv),
      done_(done) {}

void Lexer::run() {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " +
                             filePath.filename().string());
  }

  int lexerCount = 0;
  std::string line;

  while (std::getline(file, line)) {
    std::vector<Token> tokenized_line;
    std::vector<int> math_op;
    std::vector<int> equal_op;
    int swap_op = -1;
    int return_swap_op = -1;
    bool skip = false;
    for (int i = 0; i < line.size(); i++) {
      char current = line[i];
      if (isIdentifierStart(current)) {
        Token temp = readIdentifierOrKeyword(line, i);
        if (temp.tokenType == TokenType::PRINT ||
            temp.tokenType == TokenType::READ ||
            temp.tokenType == TokenType::FUNCTION ||
            temp.tokenType == TokenType::IF ||
            temp.tokenType == TokenType::WHILE ||
            temp.tokenType == TokenType::FOR) {
          swap_op = tokenized_line.size();
        } else if (temp.tokenType == TokenType::RETURN)
          return_swap_op = tokenized_line.size();
        tokenized_line.push_back(temp);
      } else if (std::isdigit(current) ||
                 (current == '-' && std::isdigit(line[i + 1]))) {
        tokenized_line.push_back(readNumber(line, i));
      }
      current = line[i];
      switch (current) {
        case '"': {
          int start = i++;
          while (line[i] != '"') {
            i++;
            if (i >= line.size())
              throw std::invalid_argument("Missing \" at: " + line);
          }
          tokenized_line.emplace_back(TokenType::WORD_CONST,
                                      line.substr(start, i - start + 1));
        } break;
        case '#':
          skip = true;
          break;
        case '.':
          tokenized_line.emplace_back(TokenType::LINKER, ".");
          continue;
        case '+':
          tokenized_line.emplace_back(TokenType::PLUS, "+");
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '-':
          tokenized_line.emplace_back(TokenType::MINUS, "-");
          continue;
        case '/':
          tokenized_line.emplace_back(TokenType::DIVIDE, "/");
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '%':
          tokenized_line.emplace_back(TokenType::MODE, "%");
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '*':
          if (i + 1 < line.size() && line[i + 1] == '*') {
            tokenized_line.emplace_back(TokenType::POW, "**");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::TIMES, "*");
          }
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '>':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::E_GREATER, ">=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::GREATER, ">");
          }
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '<':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::E_SMALLER, "<=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::SMALLER, "=");
          }
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '&':
          tokenized_line.emplace_back(TokenType::AND, "&");
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '|':
          tokenized_line.emplace_back(TokenType::OR, "|");
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '^':
          tokenized_line.emplace_back(TokenType::XOR, "^");
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '!':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::NOT_EQUIVALENT, "!=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::NOT, "!");
          }
          math_op.push_back(tokenized_line.size() - 1);
          continue;
        case '=':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::EQUIVALENT, "==");
            math_op.push_back(tokenized_line.size() - 1);
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::EQUAL, "=");
            equal_op.push_back(tokenized_line.size() - 1);
          }
          continue;
        case ':':
          tokenized_line.emplace_back(TokenType::COLON, ":");
          continue;
        case ',':
          tokenized_line.emplace_back(TokenType::COMMA, ",");
          continue;
        case '[':
          tokenized_line.emplace_back(TokenType::L_SQBACKET, "[");
          continue;
        case ']':
          tokenized_line.emplace_back(TokenType::R_SQBACKET, "]");
          continue;
        case '(':
          tokenized_line.emplace_back(TokenType::L_RBACKET, "(");
          continue;
        case ')':
          tokenized_line.emplace_back(TokenType::R_RBACKET, ")");
          continue;
        case '{':
          tokenized_line.emplace_back(TokenType::L_SBACKET, "{");
          continue;
        case '}':
          tokenized_line.emplace_back(TokenType::R_SBACKET, "}");
          continue;
        case ' ':
          continue;
      }
      if (skip) {
        break;
      }
    }

    for (auto it : math_op) {
      if (tokenized_line[it + 1].tokenType != TokenType::L_RBACKET) {
        std::swap(tokenized_line[it], tokenized_line[it + 1]);
      } else {
        int count = 1;
        int i = 1;
        while (count != 0 && it + i < tokenized_line.size()) {
          i++;
          if (tokenized_line[it + i].tokenType == TokenType::L_RBACKET) count++;
          if (tokenized_line[it + i].tokenType == TokenType::R_RBACKET) count--;
        }
        tokenized_line.insert(tokenized_line.begin() + it + i,
                              tokenized_line[it]);
        tokenized_line.erase(tokenized_line.begin() + it);
      }
    }
    for (auto op : equal_op) {
        bool done = false;
        for (auto i = tokenized_line.begin() + op + 1; i < tokenized_line.end();
             i++) {
          if (i->tokenType == TokenType::COLON) {
            auto it = tokenized_line.begin() + op;
            tokenized_line.insert(i, *it);
            tokenized_line.insert(i+1, *(it - 1));
            done = true;
            break;
          }
        }
        if (!done) {
          tokenized_line.push_back(tokenized_line[op]);
          tokenized_line.push_back(tokenized_line[op - 1]);
        }
        tokenized_line.erase(tokenized_line.begin() + op);
        tokenized_line.erase(tokenized_line.begin() + (op - 1));

        if (swap_op != -1 && swap_op > op) swap_op -= 2;
    }

    if (swap_op != -1) {
      int l_rbacket = 0, r_rbacket = 0;
      int index = 1;
      int end = -1;
      while (swap_op + index < tokenized_line.size()) {
        if (tokenized_line[swap_op + index].tokenType == TokenType::R_RBACKET)
          r_rbacket++;
        if (tokenized_line[swap_op + index].tokenType == TokenType::L_RBACKET)
          l_rbacket++;
        if (l_rbacket == r_rbacket && l_rbacket > 0) {
          end = swap_op + index + 1;
          break;
        }
        index++;
      }
      if (end != -1) {
        tokenized_line.insert(tokenized_line.begin() + end,
                              tokenized_line[swap_op]);
        tokenized_line.erase(tokenized_line.begin() + swap_op);
      }
    }

    if (return_swap_op != -1) {
      tokenized_line.push_back(tokenized_line[return_swap_op]);
      tokenized_line.erase(tokenized_line.begin() + return_swap_op);
    }

    tokenized_line = reorderExpression(tokenized_line);

    {
      std::lock_guard<std::mutex> lock(mutex_);
      tokenized_code.push_back(tokenized_line);
    }
  }

  lexerCount++;
  if (lexerCount >= 10) {
    cv_.notify_one();
  }
  {
    std::lock_guard<std::mutex> lock(mutex_);
    done_ = true;
  }
  cv_.notify_all();
  file.close();
}

Token Lexer::readIdentifierOrKeyword(const std::string& line, int& i) {
  int start = i;
  while (i < line.size() && isIdentifierPart(line[i])) {
    i++;
  }

  std::string word = line.substr(start, i - start);
  if (keywords.count(word)) {
    return {keywords[word], word};
  } else if (line[i] == '(') {
    return {TokenType::FUNCTION, word};
  } else {
    return {TokenType::VARIABLE, word};
  }
}

Token Lexer::readNumber(const std::string& line, int& i) {
  int start = i++;
  bool hasDecimalPoint = false;

  while (i < line.size() && (std::isdigit(line[i]) || line[i] == '.')) {
    if (line[i] == '.') {
      if (hasDecimalPoint)
        break;  // Stop if there's more than one decimal point
      hasDecimalPoint = true;
    }
    ++i;
  }

  return {TokenType::NUM_CONST, line.substr(start, i - start)};
}

bool Lexer::isIdentifierStart(char c) { return std::isalpha(c) || c == '_'; }

bool Lexer::isIdentifierPart(char c) { return std::isalnum(c) || c == '_'; }

int Lexer::getPrecedence(TokenType type) {
  switch (type) {
    case TokenType::OR:
    case TokenType::XOR:
      return 1;
    case TokenType::AND:
      return 2;
    case TokenType::EQUAL:
    case TokenType::NOT_EQUIVALENT:
    case TokenType::EQUIVALENT:
    case TokenType::GREATER:
    case TokenType::SMALLER:
    case TokenType::E_GREATER:
    case TokenType::E_SMALLER:
      return 3;
    case TokenType::PLUS:
    case TokenType::MINUS:
      return 4;
    case TokenType::TIMES:
    case TokenType::DIVIDE:
    case TokenType::MODE:
      return 5;
    case TokenType::POW:
      return 6;
    default:
      return 0;
  }
}

bool Lexer::isLeftAssociative(TokenType type) {
  return type != TokenType::POW;  // Exponentiation is right associative
}

std::vector<Token> Lexer::reorderExpression(const std::vector<Token>& vec) {
  std::stack<Token> operatorStack;
  std::vector<Token> outputQueue;
  std::vector<Token> finalOutput;
  bool encounteredEqual = false;

  for (const auto& token : vec) {
    if (token.tokenType == TokenType::COLON) {
      // Push current segment to final output and clear for next segment
      while (!operatorStack.empty()) {
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
      }
      finalOutput.insert(finalOutput.end(), outputQueue.begin(),
                         outputQueue.end());
      finalOutput.push_back(token);  // Add the colon back
      outputQueue.clear();
      encounteredEqual = false;  // Reset for next segment
      continue;
    }

    if (encounteredEqual) {
      outputQueue.push_back(token);
      continue;
    }

    if (token.tokenType == TokenType::EQUAL) {
      while (!operatorStack.empty()) {
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
      }
      encounteredEqual = true;
      outputQueue.push_back(token);
    } else if (token.tokenType == TokenType::NUM_CONST ||
               token.tokenType == TokenType::VARIABLE ||
               token.tokenType == TokenType::WORD_CONST) {
      outputQueue.push_back(token);
    } else if (token.tokenType == TokenType::PLUS ||
               token.tokenType == TokenType::MINUS ||
               token.tokenType == TokenType::TIMES ||
               token.tokenType == TokenType::DIVIDE ||
               token.tokenType == TokenType::MODE ||
               token.tokenType == TokenType::POW ||
               token.tokenType == TokenType::GREATER ||
               token.tokenType == TokenType::SMALLER ||
               token.tokenType == TokenType::E_GREATER ||
               token.tokenType == TokenType::E_SMALLER ||
               token.tokenType == TokenType::EQUIVALENT ||
               token.tokenType == TokenType::NOT_EQUIVALENT ||
               token.tokenType == TokenType::AND ||
               token.tokenType == TokenType::OR ||
               token.tokenType == TokenType::XOR) {
      while (!operatorStack.empty() &&
             (getPrecedence(operatorStack.top().tokenType) >
                  getPrecedence(token.tokenType) ||
              (getPrecedence(operatorStack.top().tokenType) ==
                   getPrecedence(token.tokenType) &&
               isLeftAssociative(token.tokenType))) &&
             operatorStack.top().tokenType != TokenType::L_RBACKET) {
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
      }
      operatorStack.push(token);
    } else if (token.tokenType == TokenType::L_RBACKET) {
      operatorStack.push(token);
    } else if (token.tokenType == TokenType::R_RBACKET) {
      while (!operatorStack.empty() &&
             operatorStack.top().tokenType != TokenType::L_RBACKET) {
        outputQueue.push_back(operatorStack.top());
        operatorStack.pop();
      }
      if (!operatorStack.empty()) {
        operatorStack.pop();  // Pop the left bracket
      }
    } else {
      outputQueue.push_back(token);
    }
  }

  // Final flush of remaining operators
  while (!operatorStack.empty()) {
    outputQueue.push_back(operatorStack.top());
    operatorStack.pop();
  }

  // Add remaining output to final result
  finalOutput.insert(finalOutput.end(), outputQueue.begin(), outputQueue.end());

  // Remove parentheses from finalOutput
  finalOutput.erase(
      std::remove_if(finalOutput.begin(), finalOutput.end(),
                     [](const Token& token) {
                       return token.tokenType == TokenType::L_RBACKET ||
                              token.tokenType == TokenType::R_RBACKET;
                     }),
      finalOutput.end());

  return finalOutput;
}