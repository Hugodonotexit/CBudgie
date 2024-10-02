#include "Lexer.h"

#include <iostream>

Lexer::Lexer(const std::filesystem::path& filePath,
             std::vector<std::vector<Token>>& tokenized_code)
    : filePath(filePath),
      tokenized_code(tokenized_code){}

void Lexer::run() {
  std::ifstream file(filePath);

  // Attempt to open file
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filePath.filename().string());
  }

  std::string strline;
  std::vector<std::string> lines;
  // Loop through each line of the file and convert to tokens
  while (std::getline(file, strline)) {
    lines.push_back(strline);
  }

  file.close();

  tokenized_code.resize(lines.size());

  #pragma omp parallel for
  for (int i = 0; i < lines.size(); ++i) {
      tokenized_code[i] = Lexer::preprocessLine(lines[i]);
  }

}

std::vector<Token> Lexer::preprocessLine(std::string line) {
    std::vector<Token> tokenized_line;
    std::vector<int> math_op;
    std::vector<int> equal_op;
    int swap_op = -1;
    int return_swap_op = -1;
    bool skip = false;

    // Sweep through each char in the line
    for (int i = 0; i < line.size(); i++) {
      char current = line[i];
      char next = line[i+1];
      // Determine if the current charater could be the start of a keyword
      if (isIdentifierStart(current, next)) {
        // Collect token type and string of identifier
        Token temp = readIdentifierOrKeyword(line, i);
        // Check if token is a non return keyword or built in function
        if(isKewordOrInBuiltFunction(temp.tokenType)){
          swap_op = tokenized_line.size();
        } 
        else if (temp.tokenType == TokenType::RETURN) {
          return_swap_op = tokenized_line.size();
        }
        //Add token,string object to converted line
        tokenized_line.push_back(temp);
      } 
      // Determine if the current charater could be the start of a number
      else if (isNumberStart(current, next)) {
        //Addtoken string object to converted line
        tokenized_line.push_back(readNumber(line, i));
      } 

      if (i >= line.size()) break;
      // reset current
      current = line[i];
      next = line[i+1];
      // Pretty much same as before but looking at all single char identifiers
      switch (current) {
        case 'f': {
          i++;
          int start = i++;
          // Collect entire string
          while (line[i] != '"') {
            i++;
            if (i >= line.size())
              //String not closed (note strings cannot be multi line)
              throw std::invalid_argument("Missing \" at: " + line);
          }
          int strLength = i - start + 1;

          std::string str = line.substr(start, strLength);
          int lastEnd = -1;
          for (int j = 1; j < strLength; j++) {
            if (str.substr(j,2) == "\\{"){
              j++;
              continue;
            } 
            else if (str[j] == '{') {
              start = j;
              while (str[j] != '}') {
                j++;
                if (str.substr(j,2) == "\\}") j++;
                if (j >= str.size())
                  // String escape not closed (note strings cannot be multi line)
                  throw std::invalid_argument("Missing } at: " + line);
              }
              std::vector<Token> fstringVar = preprocessLine(str.substr(start+1,j-start-1));
              if (lastEnd != -1) {
                tokenized_line.emplace_back(TokenType::PLUS, "+");
                math_op.push_back(tokenized_line.size() - 1);
                tokenized_line.emplace_back(TokenType::WORD_CONST, "\""+str.substr(lastEnd,  start - lastEnd)+"\"");
              }else{
                tokenized_line.emplace_back(TokenType::WORD_CONST, str.substr(0, start)+ "\"");
              }

              tokenized_line.emplace_back(TokenType::PLUS, "+");
              math_op.push_back(tokenized_line.size() - 1);
              
              tokenized_line.insert(tokenized_line.end(), fstringVar.begin(), fstringVar.end());
              lastEnd = j+1;
              if (j == str.size()-1 && lastEnd < j) {
                tokenized_line.emplace_back(TokenType::PLUS, "+");
                math_op.push_back(tokenized_line.size() - 1);
                tokenized_line.emplace_back(TokenType::WORD_CONST, "\""+str.substr(lastEnd));
              }
            }
          }
        }
          break;
        case '"': {
          int start = i++;
          // Collect entire string
          while (line[i] != '"') {
            i++;
            if (i >= line.size())
              //String not closed (note strings cannot be multi line)
              throw std::invalid_argument("Missing \" at: " + line);
          }          
          tokenized_line.emplace_back(TokenType::WORD_CONST, line.substr(start, i - start + 1));
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
      // skipping comment
      if (skip) {
        break;
      }
    }

    // loop through all math operations
    for (auto it : math_op) {
      if (tokenized_line[it + 1].tokenType == TokenType::L_RBACKET) {
        // otherwise collect rest of the bracket including sub brackets
        int count = 1;
        int i = 1;
        while (count != 0 && it + i < tokenized_line.size()) {
          i++;
          if (tokenized_line[it + i].tokenType == TokenType::L_RBACKET) count++;
          if (tokenized_line[it + i].tokenType == TokenType::R_RBACKET) count--;
        }
        //move bracketed stuff to the front of the queue
        tokenized_line.insert(tokenized_line.begin() + it + i + 1, tokenized_line[it]);
        tokenized_line.erase(tokenized_line.begin() + it, tokenized_line.begin() + it + 1);
        
      } else if (tokenized_line[it + 2].tokenType == TokenType::L_SQBACKET) {
        // otherwise collect rest of the bracket including sub brackets
        int count = 1;
        int i = 2;
        while (count != 0 && it + i < tokenized_line.size()) {
          i++;
          if (tokenized_line[it + i].tokenType == TokenType::L_SQBACKET) count++;
          if (tokenized_line[it + i].tokenType == TokenType::R_SQBACKET) count--;
        }
        //move bracketed stuff to the front of the queue
        tokenized_line.insert(tokenized_line.begin() + it + i + 1, tokenized_line[it]);
        tokenized_line.erase(tokenized_line.begin() + it, tokenized_line.begin() + it + 1);
        
      } else {
        // if math operation is not next to a bracket arrange the tokens so the order goes from num,op,num to num,num,op
        std::swap(tokenized_line[it], tokenized_line[it + 1]);
      }
    }
    
    //loop through all comparasion operations
    for (auto op : equal_op) {
        std::vector<Token>::iterator colonIndex = tokenized_line.end();
        for (auto i = tokenized_line.begin() + op + 1; i < tokenized_line.end();i++) {
          if (i->tokenType == TokenType::COLON) {
            colonIndex = i;
            break;
          }
        }
        if (tokenized_line[op-1].tokenType != TokenType::VARIABLE)
        {
            int count = 1;
            int j = -1;
            while (count != 0 && tokenized_line.begin() + (op + j)!= tokenized_line.begin()) {
                j--;
                if ((tokenized_line.begin() + (op + j))->tokenType == TokenType::L_SQBACKET) count--;
                if ((tokenized_line.begin() + (op + j))->tokenType == TokenType::R_SQBACKET) count++;
            }
            j--;
            if ((tokenized_line.begin() + (op + j))->tokenType != TokenType::VARIABLE) throw std::invalid_argument("Missing variable");
            if (colonIndex !=  tokenized_line.end()) {
                tokenized_line.insert(colonIndex, *(tokenized_line.begin() + op));
                tokenized_line.insert(colonIndex+1, tokenized_line.begin() + (op + j), tokenized_line.begin() + op);
            } else {
                tokenized_line.push_back(*(tokenized_line.begin() + op));
                std::vector<Token> temp;
                temp.insert(temp.end(), tokenized_line.begin() + (op + j), tokenized_line.begin() + op);
                tokenized_line.insert(tokenized_line.end(), temp.begin(), temp.end());
            }
            
            tokenized_line.erase(tokenized_line.begin() + (op + j), tokenized_line.begin() + op + 1);
            
            if (swap_op != -1 && swap_op > op) swap_op -= (1-j);
            continue;
        }
            
        if (colonIndex !=  tokenized_line.end()) {
          auto it = tokenized_line.begin() + op;
          tokenized_line.insert(colonIndex, *it);
          tokenized_line.insert(colonIndex+1, *(it - 1));
        } else {
          tokenized_line.push_back(tokenized_line[op]);
          tokenized_line.push_back(tokenized_line[op - 1]);
        }
        tokenized_line.erase(tokenized_line.begin() + (op - 1), tokenized_line.begin() + op + 1);

        if (swap_op != -1 && swap_op > op) swap_op -= 2;
    }

    // Move all tokens that return true on isKewordOrInBuiltFunction to after the brackets after them if there
    if (swap_op != -1) {
      int l_rbacket = 0, r_rbacket = 0;
      int index = 1;
      int end = -1;
      // Identify end of bracket
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
      // If end is -1 there are no brackets
      if (end != -1) {
        // There are brackets move token
        tokenized_line.insert(tokenized_line.begin() + end,
                              tokenized_line[swap_op]);
        tokenized_line.erase(tokenized_line.begin() + swap_op);
      }
    }

    // Move return token to end of line if there is one
    if (return_swap_op != -1) {
      tokenized_line.push_back(tokenized_line[return_swap_op]);
      tokenized_line.erase(tokenized_line.begin() + return_swap_op);
    }

    std::vector<int> list_swap_op;
    for (int i = 1; i < tokenized_line.size(); i++)
    {
      if (tokenized_line[i].tokenType == TokenType::L_SQBACKET && tokenized_line[i-1].tokenType == TokenType::VARIABLE) list_swap_op.push_back(i-1);
    }

    for (auto it:list_swap_op)
    {
      int count = 1;
      int i = 1;
      while (count != 0 && it + i < tokenized_line.size()) {
        i++;
        if (tokenized_line[it + i].tokenType == TokenType::L_SQBACKET) count++;
        else if (tokenized_line[it + i].tokenType == TokenType::R_SQBACKET) count--;
      }
      //move bracketed stuff to the front of the queue
      if (it - 1 >= 0 && tokenized_line[it - 1].tokenType == TokenType::EQUAL)
      {
        tokenized_line.insert(tokenized_line.begin() + it + i + 1, tokenized_line.begin() + it - 1, tokenized_line.begin() + it + 1);
        tokenized_line.erase(tokenized_line.begin() + it - 1, tokenized_line.begin() + it + 1);
      } else {
        tokenized_line.insert(tokenized_line.begin() + it + i + 1, tokenized_line[it]);
        tokenized_line.erase(tokenized_line.begin() + it);
      }
      
      
    }

    tokenized_line = reorderExpression(tokenized_line);

    return tokenized_line;
}

Token Lexer::readIdentifierOrKeyword(const std::string& line, int& i) {
  int start = i;
  // progess i until it is the final char in the identifier
  while (i < line.size() && isIdentifierPart(line[i])) {
    i++;
  }

  //Return the associated token type along with the string of the identifier
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

  //Return the associated token type along with the string of the number
  return {TokenType::NUM_CONST, line.substr(start, i - start)};
}

bool Lexer::isIdentifierStart(char c, char next) {
  if (c == 'f' && next == '\"')
  {
    return false;
  }
   
  return  std::isalpha(c) || c == '_'; 
  }

bool Lexer::isIdentifierPart(char c) { return std::isalnum(c) || c == '_'; }

bool Lexer::isNumberStart(char current,char next){
  return std::isdigit(current) || (current == '-' && std::isdigit(next));
}

bool Lexer::isKewordOrInBuiltFunction(TokenType type){
  return    type == TokenType::PRINT ||
            type == TokenType::READ ||
            type == TokenType::FUNCTION ||
            type == TokenType::NUM ||
            type == TokenType::IF ||
            type == TokenType::WHILE ||
            type == TokenType::FOR;
}

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
    if (token.tokenType == TokenType::COLON || token.tokenType == TokenType::COMMA) {
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