#include "parser.h"
BoolSupport::BoolSupport(vector<vector<Token>> &tokens) : Parser_var(tokens) {}
BoolSupport::~BoolSupport() {}

 void BoolSupport::scanFunc() {
  cleanBracket();
  cleanNAN();
  locateBrackets();
  locateOps();
}

 void BoolSupport::cleanNAN() {
#pragma omp parallel for
  for (int i = 0; i < (int)tokens_copy.size(); i++) {
        if (tokens_copy[i].type == TokenType::NA)
        {
            tokens_copy.erase(tokens_copy.begin() + i);
            i--;
        }
  }
};

 void BoolSupport::cleanBracket() {
 #pragma omp parallel for
  for (int i = 0; i < (int)tokens_copy.size()-2; i++) {
      if (tokens_copy[i].type == TokenType::L_RBACKET &&
          tokens_copy[i + 2].type == TokenType::R_RBACKET) {
        tokens_copy[i].type = TokenType::NA;
        tokens_copy[i + 2].type = TokenType::NA;
      }
  }
}

 void BoolSupport::locateOps() {
  operators[0].erase(operators[0].begin(), operators[0].end());
  operators[1].erase(operators[1].begin(), operators[1].end());
  for (int i = 0; i < (int)tokens_copy.size(); i++) {
      switch (tokens_copy[i].type)
      {
      case TokenType::XOR:
      case TokenType::AND:
      case TokenType::OR:
      case TokenType::NOT:
        operators[1].push_back(i);
        break;
      case TokenType::EQUIVALENT:
      case TokenType::E_GREATER:
      case TokenType::E_SMALLER:
      case TokenType::GREATER:
      case TokenType::SMALLER:
        operators[0].push_back(i);
        break;
      } 
  }
}

 void BoolSupport::locateBrackets() {
  brackets.erase(brackets.begin(), brackets.end());
  std::vector<int> openBracket, closeBracket;
  for (int i = 0; i < (int)tokens_copy.size(); i++) {
      if (tokens_copy[i].type == TokenType::L_RBACKET) {
        openBracket.push_back(i);
      } else if (tokens_copy[i].type == TokenType::R_RBACKET) {
        closeBracket.push_back(i);
      }
  }
  if (openBracket.size() != closeBracket.size()) {
    error(ERROR::BRACKET,-1,-1);
    return;
  }
  for (int i = (int)openBracket.size() - 1; i >= 0; i--) {
    if (openBracket[i] >= closeBracket[i]) {
      error(ERROR::OTHER,-1,-2);
      return;
    }
  }
  for (int i = 0; i < (int)openBracket.size(); i++) {
    for (int j = closeBracket.size() - 1; j >= 0; j--) {
      brackets.emplace_back(openBracket[i], closeBracket[j]);
    }
  }
}

void BoolSupport::calculate(int index) {
    switch (tokens_copy[index].type)
    {
    case TokenType::XOR:
        if (tokens_copy[index-1].type == TokenType::TRUE && tokens_copy[index+1].type == TokenType::TRUE)
        {
            tokens_copy[index].type = TokenType::FALSE;
        } else if (tokens_copy[index-1].type == TokenType::TRUE || tokens_copy[index+1].type == TokenType::TRUE)
        {
            tokens_copy[index].type = TokenType::TRUE;
        } else {tokens_copy[index].type = TokenType::FALSE;}
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
        break;
    case TokenType::NOT_EQUIVALENT:
        if (tokens_copy[index-1].value != tokens_copy[index+1].value)
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::EQUIVALENT:
        if (tokens_copy[index-1].type == TokenType::NUMBER || tokens_copy[index+1].type == TokenType::NUMBER)
        {
            if (stold(tokens_copy[index-1].value) == stold(tokens_copy[index+1].value))
            {
                tokens_copy[index].type = TokenType::TRUE;
                tokens_copy[index].value = "True";
            } else {
                tokens_copy[index].type = TokenType::FALSE;
                tokens_copy[index].value = "False";
            }
            tokens_copy[index-1].type = TokenType::NA;
            tokens_copy[index+1].type = TokenType::NA;
        } else {
            if (tokens_copy[index-1].value == tokens_copy[index+1].value)
            {
                tokens_copy[index].type = TokenType::TRUE;
                tokens_copy[index].value = "True";
            } else {
                tokens_copy[index].type = TokenType::FALSE;
                tokens_copy[index].value = "False";
            }
            tokens_copy[index-1].type = TokenType::NA;
            tokens_copy[index+1].type = TokenType::NA;
        }
        
        break;
    case TokenType::E_GREATER:
        if (tokens_copy[index-1].type != TokenType::NUMBER || tokens_copy[index+1].type != TokenType::NUMBER)
        {
            error(ERROR::OTHER,-1,-1);
            return;
        }
        if (stold(tokens_copy[index-1].value) >= stold(tokens_copy[index+1].value))
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::E_SMALLER:
        if (tokens_copy[index-1].type != TokenType::NUMBER || tokens_copy[index+1].type != TokenType::NUMBER)
        {
            error(ERROR::OTHER,-1,-1);
            return;
        }
        if (stold(tokens_copy[index-1].value) <= stold(tokens_copy[index+1].value))
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::GREATER:
        if (tokens_copy[index-1].type != TokenType::NUMBER || tokens_copy[index+1].type != TokenType::NUMBER)
        {
            error(ERROR::OTHER,-1,-1);
            return;
        }
        if (stold(tokens_copy[index-1].value) > stold(tokens_copy[index+1].value))
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::SMALLER:
        if (tokens_copy[index-1].type != TokenType::NUMBER || tokens_copy[index+1].type != TokenType::NUMBER)
        {
            error(ERROR::OTHER,-1,-1);
            return;
        }
        if (stold(tokens_copy[index-1].value) < stold(tokens_copy[index+1].value))
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::AND:
        if (tokens_copy[index-1].type == TokenType::TRUE && tokens_copy[index+1].type == TokenType::TRUE)
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
            }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::OR:
        if (tokens_copy[index-1].type == TokenType::TRUE || tokens_copy[index+1].type == TokenType::TRUE)
        {
            tokens_copy[index].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        } else {
            tokens_copy[index].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        }
        tokens_copy[index-1].type = TokenType::NA;
        tokens_copy[index+1].type = TokenType::NA;
        break;
    case TokenType::NOT:
        if (tokens_copy[index+1].type == TokenType::TRUE)
        {
            tokens_copy[index+1].type = TokenType::FALSE;
            tokens_copy[index].value = "False";
        } else {
            tokens_copy[index+1].type = TokenType::TRUE;
            tokens_copy[index].value = "True";
        }
        tokens_copy[index].type = TokenType::NA;
        break;
    }
}
