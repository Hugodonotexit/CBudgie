#include "parser.h"

Parser::Parser(vector<vector<Token>> &token) : tokens(token) {
  for (int line = 0; line < tokens.size(); line++) {
    #pragma omp parallel for
    for (int pos = 0; pos < tokens[line].size(); pos++) {
      switch (tokens[line][pos].type) {
        case TokenType::FILE:
          defFile(line, pos);
          break;
        case TokenType::DEF:
          defScope<Function>(line, pos);
          break;
        case TokenType::FOR:
          defScope<For>(line, pos);
          break;
        case TokenType::WHILE:
          defScope<While>(line, pos);
          break;
        case TokenType::IF:
          defScope<If>(line, pos);
          break;
        case TokenType::SWITCH:
          defScope<Switch>(line, pos);
          break;
      }
    }
  } 
}

std::shared_ptr<Variable> Parser::defVar(int &line, int &pos) {
  shared_ptr<Variable> var;
  TokenType *type = new TokenType;
  type = nullptr;
  #pragma omp parallel for
  for (size_t i = pos; i < tokens[line].size(); i++)
  {
    if (tokens[line][i].type == TokenType::AS)
    {
      *type = tokens[line][i+1].type;
      break;
    }
  }
  if (type == nullptr && tokens[line][pos+2].type != TokenType::EQUAL) {
    throw runtime_error("Initialise with out type at " + to_string(line) + ":" + to_string(pos));
  } else if (type != nullptr && tokens[line][pos+2].type == TokenType::EQUAL) {
      if (pos+3 < tokens[line].size()-2)
      {
        switch (tokens[line][pos+3].type)
        {
        case TokenType::TRUESTRING:
            var = make_shared<VariableType<string>>(tokens[line][pos+1].value, tokens[line][pos+3].value);
          break;
        case TokenType::NUMBER:
            long double num = stold(tokens[line][pos+3].value);
            if (floor(num) == num)
            {
              var = make_shared<VariableType<int_least16_t>>(tokens[line][pos+1].value, (int_least16_t)num);
            } else {
              var = make_shared<VariableType<double>>(tokens[line][pos+1].value, (double)num);
            }
          break;

        }
      } else {
        
      }
      
  }else if (type != nullptr && tokens[line][pos+2].type != TokenType::EQUAL)
  {
    switch (*type)
    {
    case TokenType::STRINGTYPE:
      var = make_shared<VariableType<string>>(tokens[line][pos+1].value, "0");
      break;
    case TokenType::CHAR:
      var = make_shared<VariableType<char>>(tokens[line][pos+1].value, '0');
      break;
    case TokenType::BOOL:
      var = make_shared<VariableType<bool>>(tokens[line][pos+1].value, false);
      break;
    case TokenType::WCHAR:
      var = make_shared<VariableType<wchar_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::UINT8:
      var = make_shared<VariableType<uint_least8_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::INT8:
      var = make_shared<VariableType<int_least8_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::UINT16:
      var = make_shared<VariableType<uint_least16_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::INT16:
      var = make_shared<VariableType<int_least16_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::UINT32:
      var = make_shared<VariableType<uint_least32_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::INT32:
      var = make_shared<VariableType<int_least32_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::UINT64:
      var = make_shared<VariableType<uint_least64_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::INT64:
      var = make_shared<VariableType<int_least64_t>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::FLOAT4:
      var = make_shared<VariableType<float>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::FLOAT8:
      var = make_shared<VariableType<double>>(tokens[line][pos+1].value, 0);
      break;
    case TokenType::FLOAT16:
      var = make_shared<VariableType<long double>>(tokens[line][pos+1].value, 0);
      break;   
  }
  } 
    
    delete type;
    return var;
}

void Parser::defFile(int &line, int &pos) {
    File file(tokens[line][pos+1].value);
    int i = line;
    int j = pos;
    do
    {
      j++;
      if (j == tokens[i].size()) {
      i++;
      j=0;
      if (i == tokens.size())
      {
        i--;
        j = tokens[i].size() - 1;
        break;
      }
    }
    
    } while (!(tokens[i][j].value == "`"));
    file.setEndtPos(i,j);
    scope.push_back(make_shared<File>(file));
}

template <typename T>
void Parser::defScope(int &line, int &pos) {
  int openbracket = 0;
  int closebracket = 0;
  int openRoundBracket = 0;
  int closeRoundBracket = 0;
  int i = line;
  int j = pos;
  
  std::optional<T> scoOpt;

  if constexpr (std::is_same_v<T, Function>)
  {
    scoOpt.emplace(tokens[line][pos+1].value);
  } else {
    scoOpt.emplace();
  }
  
  T& sco = scoOpt.value();
  
  
  do {
    j++;
    if (j >= tokens[i].size()) {
      i++;
      j=0;
      if (i >= tokens.size())
      {
        throw std::runtime_error("Missing close bracket at " + to_string(i) + ":" + to_string(j));
        abort();
      }
    }
    switch (tokens[i][j].type) {
      case TokenType::L_RBACKET:
        openRoundBracket++;
        break;
      case TokenType::R_RBACKET:
        closeRoundBracket++;
        break;
      case TokenType::L_SBACKET:
        openbracket++;
        break;
      case TokenType::R_SBACKET:
        closebracket++;
        break;
      default:
        break;
    }
    if (openRoundBracket == 1)
    {
      sco.setConStartPos(i,j);
    } else if (closeRoundBracket == 1) {
      sco.setConEndPos(i,j);
    } else if (openbracket == 1) {
      sco.setStartPos(i,j);
    }

    if (tokens[i][j].type == TokenType::LET)
    {
      sco.pushBackVar(defVar(i,j));
    }
    
  } while (!(openbracket == closebracket) || openbracket == 0 || closebracket == 0);
  sco.setEndtPos(i, j);
  scope.push_back(std::make_shared<T>(sco));
};

Parser::~Parser() {}