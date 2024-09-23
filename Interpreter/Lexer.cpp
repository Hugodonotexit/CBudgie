#include "Lexer.h"
#include <iostream>

Lexer::Lexer(const std::filesystem::path& filePath, std::vector<std::vector<Token>>& tokenized_code, std::mutex& mtx, std::condition_variable& cv, bool& done)
        : filePath(filePath), tokenized_code(tokenized_code), mutex_(mtx), cv_(cv), done_(done) {}

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
    int equal_op = -1;
    int swap_op = -1;
    bool skip = false;
    for (int i = 0; i < line.size(); i++) {
        char current = line[i];
      if (isIdentifierStart(current)) {
            Token temp = readIdentifierOrKeyword(line, i);
            if (temp.tokenType == TokenType::PRINT 
                || temp.tokenType == TokenType::READ
                || temp.tokenType == TokenType::FUNCTION 
                || temp.tokenType == TokenType::IF
                || temp.tokenType == TokenType::WHILE
                || temp.tokenType == TokenType::RETURN  ) 
                {swap_op = tokenized_line.size();};
            tokenized_line.push_back(temp);
        } else if (std::isdigit(current) || (current == '-' && std::isdigit(line[i+1]))) {
            tokenized_line.push_back(readNumber(line, i));
        }
        current = line[i];
      switch (current) {
        case '"':
        {
          int start = i++;
          while (line[i] != '"')
          {
            i++;
            if (i >= line.size()) throw std::invalid_argument("Missing \" at: " + line);
          }
          tokenized_line.emplace_back(TokenType::WORD_CONST, line.substr(start, i-start+1));
        }
          break;
        case '#':
          skip=true;
          break;
        case '.':
          tokenized_line.emplace_back(TokenType::LINKER, ".");
          continue;
        case '+':
          tokenized_line.emplace_back(TokenType::PLUS, "+");
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '-':
          tokenized_line.emplace_back(TokenType::MINUS, "-");
          continue;
        case '/':
          tokenized_line.emplace_back(TokenType::DIVIDE, "/");
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '%':
          tokenized_line.emplace_back(TokenType::MODE, "%");
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '*':
          if (i + 1 < line.size() && line[i + 1] == '*') {
            tokenized_line.emplace_back(TokenType::POW, "**");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::TIMES, "*");
          }
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '>':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::E_GREATER, ">=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::GREATER, ">");
          }
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '<':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::E_SMALLER, "<=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::SMALLER, "=");
          }
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '&':
          tokenized_line.emplace_back(TokenType::AND, "&");
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '|':
          tokenized_line.emplace_back(TokenType::OR, "|");
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '^':
          tokenized_line.emplace_back(TokenType::XOR, "^");
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '!':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::NOT_EQUIVALENT, "!=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::NOT, "!");
          }
          math_op.push_back(tokenized_line.size()-1);
          continue;
        case '=':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::EQUIVALENT, "==");
            math_op.push_back(tokenized_line.size()-1);
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::EQUAL, "=");
            equal_op = tokenized_line.size()-1;
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
      if (skip)
      {
        break;
      }
    }

    for (auto it:math_op)
    {
        if (tokenized_line[it+1].tokenType != TokenType::L_RBACKET) {
        std::swap(tokenized_line[it], tokenized_line[it+1]);
        } else {
            int count = 1;
            int i=1;
            while (count != 0 && it+i < tokenized_line.size())
            {
                i++;
                if(tokenized_line[it+i].tokenType == TokenType::L_RBACKET) count++;
                if(tokenized_line[it+i].tokenType == TokenType::R_RBACKET) count--;
            }
            tokenized_line.insert(tokenized_line.begin() + it + i,tokenized_line[it]);
            tokenized_line.erase(tokenized_line.begin()+it);
        }
    }
    if (equal_op != -1)
    {
        tokenized_line.push_back(tokenized_line[equal_op]);
        tokenized_line.push_back(tokenized_line[equal_op - 1]);
        tokenized_line.erase(tokenized_line.begin() + equal_op);
        tokenized_line.erase(tokenized_line.begin() + (equal_op - 1));
    }

    if (swap_op != -1)
    {
        if (equal_op != -1 && swap_op > equal_op) swap_op-=2;
        int index = 1;
        int end = -1;
        while (swap_op+index < tokenized_line.size()) {
            if (tokenized_line[swap_op+index].tokenType == TokenType::R_RBACKET) end=swap_op+index+1;
            index++;
        }
        if (end != -1) {
        tokenized_line.insert(tokenized_line.begin() + end, tokenized_line[swap_op]);
        tokenized_line.erase(tokenized_line.begin() + swap_op);
        }
    }
    

    tokenized_line = reorderExpression(tokenized_line);
    
    {
        std::lock_guard<std::mutex> lock(mutex_);
        tokenized_code.push_back(tokenized_line);
    }
    lexerCount++;
    if (lexerCount >= 15)
    {
        cv_.notify_one();
    }
    
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
    while (i < line.size() && isIdentifierPart(line[i])) { i++; }
    
    std::string word = line.substr(start, i - start);
    if (keywords.count(word)) {
        return {keywords[word], word};
    } else if (line[i] == '(') {
        return {TokenType::FUNCTION, word};
    } else {
        return {TokenType::VARIABLIE, word};
    }
}

Token Lexer::readNumber(const std::string& line, int& i) {
    int start = i++;
    bool hasDecimalPoint = false;

    while (i < line.size() && (std::isdigit(line[i]) || line[i] == '.')) {
        if (line[i] == '.') {
            if (hasDecimalPoint) break; // Stop if there's more than one decimal point
            hasDecimalPoint = true;
        }
        ++i;
    }

    return {TokenType::NUM_CONST, line.substr(start, i - start)};
}

bool Lexer::isIdentifierStart(char c) {
    return std::isalpha(c) || c == '_';
}

bool Lexer::isIdentifierPart(char c) {
    return std::isalnum(c) || c == '_';
}

std::vector<Token> Lexer::reorderExpression(const std::vector<Token>& vec) {
    std::stack<std::vector<Token>> stack;
    std::vector<Token> result;

    for (const auto& token : vec) {
        if (token.tokenType == TokenType::L_RBACKET) {
            stack.push(std::vector<Token>());
        } else if (token.tokenType == TokenType::R_RBACKET) {
            auto completed = stack.top();
            stack.pop();
            result.insert(result.end(), completed.begin(), completed.end());
        } else {
            if (!stack.empty()) {
                stack.top().push_back(token);
            } else {
                result.push_back(token);
            }
        }
    }

    return result;
}