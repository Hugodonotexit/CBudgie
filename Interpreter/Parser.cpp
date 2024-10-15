#include "Parser.h"

Parser::Parser(std::filesystem::path newFilePath,
                       std::filesystem::path destination)
    : filePath(newFilePath) {
  AST* tree;
  Lexer lexer(filePath, tokenized_code);

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
      tree->addChild(parseFunction());
    } 
    else if (have(TokenType::CLASS, "class")) 
    {
      tree->addChild(parseClass());
    }
    else if (have(TokenType::VARIABLE)) 
    {
      tree->addChild(parseExpression());
    }
    else throw cbg::ParserError("Invalid token at global scope: " + token->getValue());  
  } 
}

AST* Parser::parseFunction() { 
  AST* tree = new AST(TokenType::Function, "");
  tree->addChild(mustBe(TokenType::DEF, "def"));
  tree->addChild(mustBe(TokenType::VARIABLE));
  tree->addChild(parseParameterList());
  tree->addChild(parseSubroutineBody());
  return tree;
}

AST* Parser::parseClass() { 
  AST* tree = new AST(TokenType::Class, "");
  tree->addChild(mustBe(TokenType::CLASS, "class"));
  tree->addChild(mustBe(TokenType::VARIABLE));
  tree->addChild(parseClassSubroutineBody());
  return tree;
}

AST* Parser::parseParameterList() { 
  AST* tree = new AST(TokenType::ParameterList, "");
  tree->addChild(mustBe(TokenType::L_RBRACKET, "("));
  if (have(TokenType::VARIABLE))
  {
    tree->addChild(mustBe(TokenType::VARIABLE));
  }
  while (!have(TokenType::R_RBRACKET, ")")) {
    tree->addChild(mustBe(TokenType::COMMA, ","));
    tree->addChild(mustBe(TokenType::VARIABLE));
  }
  tree->addChild(mustBe(TokenType::R_RBRACKET, ")"));
  return tree;
}

AST* Parser::parseClassSubroutineBody() { 
  AST* tree = new AST(TokenType::SubroutineBody, "");
  tree->addChild(mustBe(TokenType::L_SBRACKET, "{"));
  while (!have(TokenType::R_SBRACKET, "}")) {
    if (have(TokenType::VARIABLE))
    {
      tree->addChild(parseClassVarDec());
    }
    else if (have(TokenType::DEF)) {
      tree->addChild(parseMethod());
    } 
    else throw cbg::ParserError("Invalid");
    
  }
  tree->addChild(mustBe(TokenType::R_SBRACKET, "}"));
  return tree;
}

AST* Parser::parseSubroutineBody() { 
  AST* tree = new AST(TokenType::SubroutineBody, "");
  tree->addChild(mustBe(TokenType::L_SBRACKET, "{"));
  while (!have(TokenType::R_SBRACKET, "}")) {

  }
  tree->addChild(mustBe(TokenType::R_SBRACKET, "}"));
  return tree;
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
    if (current()->getType() == expectedType && current()->getValue() == expectedValue) return true;
    return false;
}
bool Parser::have(TokenType expectedType) {
    return have(expectedType, current()->getValue());
}
Token* Parser::mustBe(TokenType expectedType) {
  return mustBe(expectedType, current()->getValue());
}

Token* Parser::mustBe(TokenType expectedType, std::string expectedValue) {
    if (current()->getType() == expectedType && current()->getValue() == expectedValue) {
        Token* temp = current();
        next();
        return temp;
    }
    throw cbg::ParserError("Invalid");
    return nullptr;
};

