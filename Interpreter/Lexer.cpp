#include "Lexer.h"

#include <iostream>

Lexer::Lexer(std::vector<std::string>& lines) : lines(lines) {}

std::vector<std::vector<Token>> Lexer::run() {
  std::vector<std::vector<Token>> tokenized_code;
  tokenized_code.resize(lines.size());

  #pragma omp parallel for
  for (int i = 0; i < lines.size(); ++i) {
    tokenized_code[i] = Lexer::preprocessLine(lines[i]);
  }

  return tokenized_code;
}


std::vector<Token> Lexer::preprocessLine(std::string line) {
    std::vector<Token> tokenized_line;
    bool skip = false;

    // Sweep through each char in the line
    for (int i = 0; i < line.size(); i++) {
      char current = line[i];
      char next = line[i+1];
      // Determine if the current charater could be the start of a keyword
      if (isIdentifierStart(current, next)) {
        // Collect token type and string of identifier
        Token temp = readIdentifierOrKeyword(line, i);
        if (temp.getType() == TokenType::IMPORT) {

          if (line[i++] != '\"') throw std::invalid_argument("missing \"");

          std::string subStr;

          while (i < line.size() - 1 && line[i] != '\"') subStr.push_back(line[i++]);

          if (line[i] != '\"') throw std::invalid_argument("missing \"");

          std::filesystem::path path = subStr;

          if (path.extension() != ".bg") throw std::invalid_argument(".bg");

          std::filesystem::path flattened;
          for (const auto& part : path) {
              if (!flattened.empty() && flattened != ".") {
                  flattened += '-';
              }
              if(part != ".") flattened += part.string();
          }

          std::filesystem::path carchePath = path.parent_path() / "__bgCache__" / flattened;

          auto future = std::async(std::launch::async, [path, carchePath]() {
            Interpreter interpreter(path, carchePath);
          });

          flattened.replace_extension("bbg");
          Token temp2(TokenType::WORD_CONST, flattened.c_str());
          tokenized_line.push_back(temp);
          tokenized_line.push_back(temp2);
        } else tokenized_line.push_back(temp);  //Add token,string object to converted line
      } 
      // Determine if the current charater could be the start of a number
      else if (isNumberStart(current, next)) {
        //Addtoken string object to converted line
        tokenized_line.push_back(readNumber(line, i));
      } 

      if (i >= line.size()) break;
      // reset current
      current = line[i];
      
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
                tokenized_line.emplace_back(TokenType::WORD_CONST, "\""+str.substr(lastEnd,  start - lastEnd)+"\"");
              }else{
                tokenized_line.emplace_back(TokenType::WORD_CONST, str.substr(0, start)+ "\"");
              }

              tokenized_line.emplace_back(TokenType::PLUS, "+");
              
              tokenized_line.insert(tokenized_line.end(), fstringVar.begin(), fstringVar.end());
              lastEnd = j+1;
              if (j == str.size()-1 && lastEnd < j) {
                tokenized_line.emplace_back(TokenType::PLUS, "+");
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
          tokenized_line.emplace_back(TokenType::DOT, ".");
          continue;
        case '+':
          tokenized_line.emplace_back(TokenType::PLUS, "+");
          continue;
        case '-':
          tokenized_line.emplace_back(TokenType::MINUS, "-");
          continue;
        case '/':
          tokenized_line.emplace_back(TokenType::DIVIDE, "/");
          continue;
        case '%':
          tokenized_line.emplace_back(TokenType::MODE, "%");
          continue;
        case '*':
          if (i + 1 < line.size() && line[i + 1] == '*') {
            tokenized_line.emplace_back(TokenType::POW, "**");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::TIMES, "*");
          }
          continue;
        case '>':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::E_GREATER, ">=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::GREATER, ">");
          }
          continue;
        case '<':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::E_SMALLER, "<=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::SMALLER, "=");
          }
          continue;
        case '&':
          tokenized_line.emplace_back(TokenType::AND, "&");
          continue;
        case '|':
          tokenized_line.emplace_back(TokenType::OR, "|");
          continue;
        case '^':
          tokenized_line.emplace_back(TokenType::XOR, "^");
          continue;
        case '!':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::NOT_EQUIVALENT, "!=");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::NOT, "!");
          }
          continue;
        case '=':
          if (i + 1 < line.size() && line[i + 1] == '=') {
            tokenized_line.emplace_back(TokenType::EQUIVALENT, "==");
            ++i;
          } else {
            tokenized_line.emplace_back(TokenType::EQUAL, "=");
          }
          continue;
        case ':':
          tokenized_line.emplace_back(TokenType::COLON, ":");
          continue;
        case ',':
          tokenized_line.emplace_back(TokenType::COMMA, ",");
          continue;
        case '[':
          tokenized_line.emplace_back(TokenType::L_SQBRACKET, "[");
          continue;
        case ']':
          tokenized_line.emplace_back(TokenType::R_SQBRACKET, "]");
          continue;
        case '(':
          tokenized_line.emplace_back(TokenType::L_RBRACKET, "(");
          continue;
        case ')':
          tokenized_line.emplace_back(TokenType::R_RBRACKET, ")");
          continue;
        case '{':
          tokenized_line.emplace_back(TokenType::L_SBRACKET, "{");
          continue;
        case '}':
          tokenized_line.emplace_back(TokenType::R_SBRACKET, "}");
          continue;
        case ' ':
          continue;
      }
      // skipping comment
      if (skip) {
        break;
      }
    }
    
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