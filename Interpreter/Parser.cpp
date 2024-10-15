#include "Parser.h"

Parser::Parser(std::filesystem::path newFilePath,
                       std::filesystem::path destination)
    : filePath(newFilePath) {
  AST* tree;
  Lexer lexer(filePath, tokenized_code);

  lexer.run();

  index_token = 0;
  token = tokenized_code[index_token].begin();

  parsing(tokenized_code, tree);

  Translator translator(tree, destination);

  delete tree;
}

Parser::Parser(std::filesystem::path newFilePath) {
  std::filesystem::path destination = newFilePath;
  destination.replace_extension(".bbg");
  Parser(newFilePath, destination);
}

void Parser::parsing(std::vector<std::vector<Token>>& tokenized_code, AST* tree) {
  tree = new AST(TokenType::Program, "");

  while (token != tokenized_code.back().end())
  {
    if (have(TokenType::DEF, "def"))
    {
      tree->addChild(parseSubroutine());
    } 
    else if (have(TokenType::CLASS, "class")) 
    {
      tree->addChild(parseClass());
    }
    else if (have(TokenType::VARIABLE)) 
    {
      tree->addChild(parseExpression());
    }
    else throw cbg::ParserError("Invalid token at global scope: " + token->code);  
  }
  
}

bool Parser::next() {
  token++;
  if (token == tokenized_code[index_token].end()) {
    if (index_token < tokenized_code.size() - 1)
    {
      index_token++;
      token = tokenized_code[index_token].begin();
    }
    return true;
  }
  return false;
}

Token* Parser::current() {
  return &(*token);
}
  
bool Parser::have(TokenType expectedType, std::string expectedValue) {
    if (current()->tokenType == expectedType && current()->code == expectedValue) return true;
    return false;
}
bool Parser::have(TokenType expectedType) {
    return have(expectedType, current()->code);
}
Token* Parser::mustBe(TokenType expectedType, std::string expectedValue) {
    if (current()->tokenType == expectedType && current()->code == expectedValue) {
        Token* temp = current();
        next();
        return temp;
    }
    throw std::invalid_argument("Parserazer");
    return nullptr;
};

