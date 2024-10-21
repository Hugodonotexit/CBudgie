#include "Parser.h"

std::string Parser::tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::IMPORT: return "import";

        case TokenType::DEF: return "def";
        case TokenType::CLASS: return "class";
        
        case TokenType::READ: return "read";
        case TokenType::PRINT: return "print";

        case TokenType::IF: return "if";
        case TokenType::ELSE: return "else";

        case TokenType::DO: return "do";
        case TokenType::WHILE: return "while";
        case TokenType::FOR: return "for";

        case TokenType::NUM_CONST: return "Number";
        case TokenType::TRUE: return "true";
        case TokenType::FALSE: return "false";
        case TokenType::WORD_CONST: return "String";

        case TokenType::VARIABLE: return "Variable";
        
        case TokenType::BREAK: return "break";
        case TokenType::CONTINUE: return "continue";
        case TokenType::RETURN: return "return";

        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::TIMES: return "*";
        case TokenType::DIVIDE: return "/";
        case TokenType::MODE: return "%";
        case TokenType::POW: return "**";
        case TokenType::EQUAL: return "=";

        case TokenType::GREATER: return ">";
        case TokenType::SMALLER: return "<";
        case TokenType::E_GREATER: return ">=";
        case TokenType::E_SMALLER: return "<=";
        case TokenType::AND: return "&";
        case TokenType::OR: return "|";
        case TokenType::NOT: return "!";
        case TokenType::XOR: return "^";
        case TokenType::EQUIVALENT: return "==";
        case TokenType::NOT_EQUIVALENT: return "!=";
        
        case TokenType::NUMERIC: return "numeric";

        case TokenType::COMMA: return ",";
        case TokenType::COLON: return ":";
        case TokenType::DOT: return "->";

        case TokenType::L_RBRACKET: return "(";
        case TokenType::R_RBRACKET: return ")";
        case TokenType::L_SQBRACKET: return "[";
        case TokenType::R_SQBRACKET: return "]";
        case TokenType::L_SBRACKET: return "{";
        case TokenType::R_SBRACKET: return "}";

        case TokenType::TO_STRING: return "to_string";
        case TokenType::TO_BOOL: return "to_bool";
        case TokenType::TO_NUM: return "to_num";

        case TokenType::nop: return "nop";

        // AST Node Types
        case TokenType::Program: return "Program";
        case TokenType::Class: return "Class";
        case TokenType::Function: return "Function";
        case TokenType::SubroutineBody: return "SubroutineBody";
        case TokenType::ParameterList: return "ParameterList";
        case TokenType::ReturnStatement: return "ReturnStatement";
        case TokenType::DoWhileStatement: return "DoWhileStatement";
        case TokenType::WhileStatement: return "WhileStatement";
        case TokenType::Condition: return "Condition";
        case TokenType::IfStatement: return "IfStatement";
        case TokenType::FunctionCall: return "FunctionCall";
        case TokenType::ArgumentList: return "ArgumentList";

        case TokenType::Method: return "Method";
        case TokenType::VariableDeclaration: return "VariableDeclaration";
        case TokenType::MemberAccess: return "MemberAccess";
        case TokenType::MethodCall: return "MethodCall";
        case TokenType::BuiltInFunctionCall: return "BuiltInFunctionCall";

        
    }

    return "Unknown";
}

void Parser::printAST(AST* node, int indent) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < indent; ++i) {
        std::cout << "-";
    }

    // Print the node information
    std::string nodeTypeStr = tokenTypeToString(node->getType());
    std::cout << nodeTypeStr;

    if (!node->getValue().empty()) {
        std::cout << ": " << node->getValue();
    }

    std::cout << std::endl;

    // Recursively print child nodes
    const std::vector<AST*>& children = node->getChildren();
    for (AST* child : children) {
        printAST(child, indent + 1);
    }
}

// Constructor that takes in file paths
Parser::Parser(std::vector<std::vector<Token>>& tokenized_code) : tokenized_code(tokenized_code) {
    // Initialize token traversal indices
    lineIndex = 0;
    tokenIndex = 0;
}

// The main parsing function
AST* Parser::run() {
    // Create the root of the AST
    AST* tree = new AST(TokenType::Program, "");

    // parse the tokens
    while (current() != nullptr) {
      if (have(TokenType::DEF, "def")) {
          tree->addChild(parseFunction());
      } else if (have(TokenType::CLASS, "class")) {
          tree->addChild(parseClass());
      } else if (have(TokenType::VARIABLE)) {
          tree->addChild(parseExpression());
      } else {
          throw cbg::ParserError("Invalid token at global scope: " + current()->getValue());
      }  
      next();
    }
    return tree;
}

// Function to parse a function definition
AST* Parser::parseFunction() {
    Token* defToken = mustBe(TokenType::DEF, "def");
    Token* nameToken = mustBe(TokenType::VARIABLE);
    AST* params = parseParameterList();
    AST* body = parseSubroutineBody();

    // Create a function AST node and add children
    AST* funcNode = new AST(TokenType::Function, "");
    funcNode->addChild(defToken);    // 'def' token
    funcNode->addChild(nameToken);   // Function name
    funcNode->addChild(params);      // Parameters
    funcNode->addChild(body);        // Body

    return funcNode;
}

// Function to parse a class definition
AST* Parser::parseClass() {
    Token* classToken = mustBe(TokenType::CLASS, "class");
    Token* nameToken = mustBe(TokenType::VARIABLE);
    AST* body = parseClassSubroutineBody();

    // Create a class AST node and add children
    AST* classNode = new AST(TokenType::Class, "");
    classNode->addChild(classToken);   // 'class' token
    classNode->addChild(nameToken);    // Class name
    classNode->addChild(body);         // Body

    return classNode;
}

// Function to parse parameter list
AST* Parser::parseParameterList() {
    Token* lParen = mustBe(TokenType::L_RBRACKET, "(");
    AST* params = new AST(TokenType::ParameterList, "");
    params->addChild(lParen);

    // Parse parameters if any
    if (!have(TokenType::R_RBRACKET, ")")) {
        do {
            Token* param = mustBe(TokenType::VARIABLE);
            params->addChild(param);

            if (have(TokenType::COMMA, ",")) {
                mustBe(TokenType::COMMA, ",");
            } else {
                break;
            }
        } while (true);
    }

    Token* rParen = mustBe(TokenType::R_RBRACKET, ")");
    params->addChild(rParen);
    return params;
}

// Function to parse a class subroutine body
AST* Parser::parseClassSubroutineBody() {
    Token* lBrace = mustBe(TokenType::L_SBRACKET, "{");
    AST* body = new AST(TokenType::SubroutineBody, "");
    body->addChild(lBrace);

    while (!have(TokenType::R_SBRACKET, "}")) {
        if (have(TokenType::DEF, "def")) {
            body->addChild(parseMethod()); // Parse methods
        } else if (have(TokenType::VARIABLE)) {
            body->addChild(parseExpression());
        } else if (have(TokenType::CLASS, "class")) {
            body->addChild(parseClass()); // Nested class
        } else {
            throw cbg::ParserError("Invalid token in class body: " + current()->getValue());
        }
    }

    Token* rBrace = mustBe(TokenType::R_SBRACKET, "}");
    body->addChild(rBrace);
    return body;
}

AST* Parser::parseMethod() {
    Token* defToken = mustBe(TokenType::DEF, "def");
    Token* methodName = mustBe(TokenType::VARIABLE);
    AST* params = parseParameterList();
    AST* body = parseSubroutineBody();

    AST* methodNode = new AST(TokenType::Method, "");
    methodNode->addChild(defToken);
    methodNode->addChild(methodName);
    methodNode->addChild(params);
    methodNode->addChild(body);

    return methodNode;
}

// Function to parse a subroutine body
AST* Parser::parseSubroutineBody() {
    Token* lBrace = mustBe(TokenType::L_SBRACKET, "{");
    AST* body = new AST(TokenType::SubroutineBody, "");
    body->addChild(lBrace);

    while (!have(TokenType::R_SBRACKET, "}")) {
        if (have(TokenType::VARIABLE) || have(TokenType::NUM_CONST)) {
            body->addChild(parseExpression());
        } else {
            body->addChild(parseStatements());
        }
    }

    Token* rBrace = mustBe(TokenType::R_SBRACKET, "}");
    body->addChild(rBrace);
    return body;
}

// Function to parse statements
AST* Parser::parseStatements() {
    if (have(TokenType::IF, "if")) {
        return parseIf();
    } else if (have(TokenType::WHILE, "while")) {
        return parseWhile();
    } else if (have(TokenType::DO, "do")) {
        return parseDo();
    } else if (have(TokenType::RETURN, "return")) {
        return parseReturn();
    } else if (have(TokenType::VARIABLE)) {
        // Need to decide between variable declaration and expression
        Token* lookaheadToken = lookahead(1); // Implement a lookahead function
        if (lookaheadToken && lookaheadToken->getType() == TokenType::VARIABLE) {
            return parseVariableDeclaration();
        } else {
            return parseExpression();
        }
    } 
    else if (have(TokenType::READ) || have(TokenType::PRINT)) {
        return parseBuiltInFunctionCall();
    }
    else {
        throw cbg::ParserError("Invalid statement starting with token: " + current()->getValue());
    }
}

AST* Parser::parseBuiltInFunctionCall() {
  if (have(TokenType::READ) || have(TokenType::PRINT) /* ... other built-in functions ... */) {
        // Built-in functions
        Token* functionName = mustBe(current()->getType());
        AST* args = parseArgumentList();

        AST* node = new AST(TokenType::BuiltInFunctionCall, "");
        node->addChild(functionName);
        node->addChild(args);
        if (have(TokenType::DOT))
        {
          node = parseDot(node);
        }
        
        return node;
  }
  else {
      throw cbg::ParserError("Invalid statement starting with token: " + current()->getValue());
  }
}

AST* Parser::parseVariableDeclaration() {
    Token* typeName = mustBe(TokenType::VARIABLE);
    Token* varName = mustBe(TokenType::VARIABLE);

    AST* varDeclNode = new AST(TokenType::VariableDeclaration, "");
    varDeclNode->addChild(typeName); // Type name
    varDeclNode->addChild(varName);  // Variable name

    return varDeclNode;
}

// Function to parse an if statement
AST* Parser::parseIf() {
    Token* ifToken = mustBe(TokenType::IF, "if");
    AST* condition = parseCondition();
    AST* body = parseSubroutineBody();

    AST* ifNode = new AST(TokenType::IfStatement, "");
    ifNode->addChild(ifToken);
    ifNode->addChild(condition);
    ifNode->addChild(body);

    if (have(TokenType::ELSE, "else")) {
        Token* elseToken = mustBe(TokenType::ELSE, "else");
        AST* elseBody = nullptr;
        if (have(TokenType::IF, "if")) {
            elseBody = parseIf(); // Else if
        } else {
            elseBody = parseSubroutineBody(); // Else
        }
        ifNode->addChild(elseToken);
        ifNode->addChild(elseBody);
    }

    return ifNode;
}

// Function to parse a while statement
AST* Parser::parseWhile() {
    Token* whileToken = mustBe(TokenType::WHILE, "while");
    AST* condition = parseCondition();
    AST* body = parseSubroutineBody();

    AST* whileNode = new AST(TokenType::WhileStatement, "");
    whileNode->addChild(whileToken);
    whileNode->addChild(condition);
    whileNode->addChild(body);

    return whileNode;
}

// Function to parse a do-while statement
AST* Parser::parseDo() {
    Token* doToken = mustBe(TokenType::DO, "do");
    AST* body = parseSubroutineBody();
    Token* whileToken = mustBe(TokenType::WHILE, "while");
    AST* condition = parseCondition();

    AST* doWhileNode = new AST(TokenType::DoWhileStatement, "");
    doWhileNode->addChild(doToken);
    doWhileNode->addChild(body);
    doWhileNode->addChild(whileToken);
    doWhileNode->addChild(condition);

    return doWhileNode;
}

// Function to parse a condition
AST* Parser::parseCondition() {
    Token* lParen = mustBe(TokenType::L_RBRACKET, "(");
    AST* expr = parseExpression();
    Token* rParen = mustBe(TokenType::R_RBRACKET, ")");

    AST* conditionNode = new AST(TokenType::Condition, "");
    conditionNode->addChild(lParen);
    conditionNode->addChild(expr);
    conditionNode->addChild(rParen);

    return conditionNode;
}

// Function to parse a return statement
AST* Parser::parseReturn() {
    Token* returnToken = mustBe(TokenType::RETURN, "return");
    AST* returnNode = new AST(TokenType::ReturnStatement, "");
    returnNode->addChild(returnToken);

    // Return expression is optional
    if (!have(TokenType::R_SBRACKET, "}") && !isAtEndOfLine()) {
        AST* expr = parseExpression();
        returnNode->addChild(expr);
    }

    return returnNode;
}

// Function to parse an expression
AST* Parser::parseExpression() {
    // Start with parsing an assignment expression
    return parseAssignment();
}

// Function to parse assignments
AST* Parser::parseAssignment() {
    AST* left = parseLogicalOrExpression();

    if (have(TokenType::EQUAL, "=")) {
        Token* opToken = mustBe(TokenType::EQUAL, "="); // Consume '='
        AST* right = parseAssignment(); // Right side may be another assignment
        opToken->addChild(left);
        opToken->addChild(right);
        return opToken;
    }

    return left;
}

// Function to parse logical OR expressions (you can extend this based on your language)
AST* Parser::parseLogicalOrExpression() {
    AST* node = parseLogicalXorExpression();

    while (have(TokenType::OR, "|")) {
        Token* opToken = mustBe(TokenType::OR, "|");
        AST* right = parseLogicalXorExpression();
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}

// Function to parse logical XOR expressions
AST* Parser::parseLogicalXorExpression() {
    AST* node = parseLogicalAndExpression();

    while (have(TokenType::XOR, "^")) {
        Token* opToken = mustBe(TokenType::XOR, "^");
        AST* right = parseLogicalAndExpression();
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}

// Function to parse logical AND expressions
AST* Parser::parseLogicalAndExpression() {
    AST* node = parseEqualityExpression();

    while (have(TokenType::AND, "&")) {
        Token* opToken = mustBe(TokenType::AND, "&");
        AST* right = parseEqualityExpression();
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}

// Function to parse equality expressions
AST* Parser::parseEqualityExpression() {
    AST* node = parseRelationalExpression();

    while (have(TokenType::EQUIVALENT, "==") || have(TokenType::NOT_EQUIVALENT, "!=")) {
        Token* opToken = mustBe(current()->getType()); // '==' or '!='
        AST* right = parseRelationalExpression();
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}

// Function to parse relational expressions
AST* Parser::parseRelationalExpression() {
    AST* node = parseAdditiveExpression();

    while (have(TokenType::GREATER) || have(TokenType::SMALLER)
           || have(TokenType::E_GREATER) || have(TokenType::E_SMALLER)) {
        Token* opToken = mustBe(current()->getType()); // '>', '<', '>=', '<='
        AST* right = parseAdditiveExpression();
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}

// Function to parse additive expressions
AST* Parser::parseAdditiveExpression() {
    AST* node = parseMultiplicativeExpression();

    while (have(TokenType::PLUS, "+") || have(TokenType::MINUS, "-")) {
        Token* opToken = mustBe(current()->getType()); // '+' or '-'
        AST* right = parseMultiplicativeExpression();

        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken; // The operator becomes the new root
    }

    return node;
}

// Function to parse multiplicative expressions
AST* Parser::parseMultiplicativeExpression() {
    AST* node = parseUnaryExpression();

    while (have(TokenType::TIMES, "*") || have(TokenType::DIVIDE, "/") || have(TokenType::MODE, "%")) {
        Token* opToken = mustBe(current()->getType()); // '*', '/', '%',
        AST* right = parseUnaryExpression();
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}

// Function to parse unary expressions
AST* Parser::parseUnaryExpression() {
    if (have(TokenType::NOT, "!") || have(TokenType::MINUS, "-")) {
        Token* opToken = mustBe(current()->getType()); // '!' or '-'
        AST* operand = parseUnaryExpression(); // Note: Recursive call for unary operators
        opToken->addChild(operand);
        return opToken;
    } else {
        return parseExponentiationExpression();
    }
}

AST* Parser::parseExponentiationExpression() {
    AST* node = parsePrimaryExpression();

    // Handle right-associative exponentiation
    if (have(TokenType::POW, "**")) {
        Token* opToken = mustBe(TokenType::POW, "**");
        AST* right = parseExponentiationExpression(); // Recursive call for right-associativity
        opToken->addChild(node);
        opToken->addChild(right);
        node = opToken;
    }

    return node;
}


AST* Parser::parseDot(AST* node) {
  // Member access or method call
  mustBe(TokenType::DOT);
  //built-in method
  if(have(TokenType::TO_BOOL) || have(TokenType::TO_NUM) || have(TokenType::TO_STRING)) {
    Token* memberName = mustBe(current()->getType());
    AST* methodCallNode = new AST(TokenType::MethodCall, "");
    mustBe(TokenType::L_RBRACKET, "(");
    mustBe(TokenType::R_RBRACKET, ")");
    methodCallNode->addChild(node);
    methodCallNode->addChild(memberName);       
    return methodCallNode;
  } else if (have(TokenType::NUMERIC)) {
    Token* memberName = mustBe(TokenType::NUMERIC);
    AST* methodCallNode = new AST(TokenType::MethodCall, "");
    AST* args = parseArgumentList();
    methodCallNode->addChild(node);       // Object
    methodCallNode->addChild(memberName); // Method name
    methodCallNode->addChild(args);
  } else {
    Token* memberName = mustBe(TokenType::VARIABLE);
    if (have(TokenType::L_RBRACKET, "(")) {
      // Method call
      AST* args = parseArgumentList();

      AST* methodCallNode = new AST(TokenType::MethodCall, "");
      methodCallNode->addChild(node);       // Object
      methodCallNode->addChild(memberName); // Method name
      methodCallNode->addChild(args);
      return methodCallNode;
    } else {
        // Member access
        AST* memberAccessNode = new AST(TokenType::MemberAccess, "");
        memberAccessNode->addChild(node);    // Object
        memberAccessNode->addChild(memberName);
        return memberAccessNode;
    }
  }
  return nullptr;
}

// Function to parse primary expressions (variables, constants, parenthesized expressions)
AST* Parser::parsePrimaryExpression() {
    AST* node = nullptr;

    if (have(TokenType::NUM_CONST)) {
        // Number constants
        node = mustBe(TokenType::NUM_CONST);
    } else if (have(TokenType::WORD_CONST)) {
        // String constants
        node = mustBe(TokenType::WORD_CONST);
    } else if (have(TokenType::READ) /* ... other built-in functions ... */) {
        node = parseBuiltInFunctionCall();
    } else if (have(TokenType::VARIABLE)) {
        // Variables, function calls, method calls, member access
        node = mustBe(TokenType::VARIABLE);

        while (have(TokenType::DOT) || have(TokenType::L_RBRACKET, "(")) {
            if (have(TokenType::DOT)) {
                node = parseDot(node);
            } else if (have(TokenType::L_RBRACKET, "(")) {
                // Function call
                AST* args = parseArgumentList();

                AST* functionCallNode = new AST(TokenType::FunctionCall, "");
                functionCallNode->addChild(node);  // Function name
                functionCallNode->addChild(args);
                node = functionCallNode;
            }
        }
    } else if (have(TokenType::L_RBRACKET, "(")) {
        // Parenthesized expression
        mustBe(TokenType::L_RBRACKET, "(");
        node = parseExpression();
        mustBe(TokenType::R_RBRACKET, ")");
    } else {
        throw cbg::ParserError("Expected primary expression, got: " + (current() ? current()->getValue() : "EOF"));
    }

    return node;
}

// Function to parse argument list (for function calls)
AST* Parser::parseArgumentList() {
    Token* lParen = mustBe(TokenType::L_RBRACKET, "(");
    AST* args = new AST(TokenType::ArgumentList, "");
    args->addChild(lParen);

    // Parse arguments if any
    if (!have(TokenType::R_RBRACKET, ")")) {
        do {
            AST* arg = parseExpression();
            args->addChild(arg);

            if (have(TokenType::COMMA, ",")) {
                mustBe(TokenType::COMMA, ",");
            } else {
                break;
            }
        } while (true);
    }

    Token* rParen = mustBe(TokenType::R_RBRACKET, ")");
    args->addChild(rParen);
    return args;
}

// Function to check if we're at the end of a line (since lines are significant)
bool Parser::isAtEndOfLine() {
    return tokenIndex >= tokenized_code[lineIndex].size();
}

// Move to the next token
bool Parser::next() {
    tokenIndex++;
    if (lineIndex >= tokenized_code.size()) return false;
    // If we've reached the end of the current line
    if (tokenIndex >= tokenized_code[lineIndex].size()) {
        // Move to the next line if not at the end of tokens
        lineIndex++;
        tokenIndex = 0;

        // Skip empty lines (if any)
        while (lineIndex < tokenized_code.size() - 1 && tokenized_code[lineIndex].empty()) {
            lineIndex++;
        }

        return false;
    }
    return true;
}

// Get the current token
Token* Parser::current() {
    if (lineIndex < tokenized_code.size() && tokenIndex < tokenized_code[lineIndex].size()) {
        return &tokenized_code[lineIndex][tokenIndex];
    }
    return nullptr; // End of tokens
}

// Function to check if the current token matches the expected type and value
bool Parser::have(TokenType expectedType, const std::string& expectedValue) {
    Token* curToken = current();
    return curToken && curToken->getType() == expectedType && curToken->getValue() == expectedValue;
}

// Function to check if the current token matches the expected type
bool Parser::have(TokenType expectedType) {
    Token* curToken = current();
    return curToken && curToken->getType() == expectedType;
}

// Function to consume the current token if it matches the expected type and value
Token* Parser::mustBe(TokenType expectedType, const std::string& expectedValue) {
    if (have(expectedType, expectedValue)) {
        Token* temp = new Token(*current());
        next();
        return temp;
    }
    std::string str = (current() ? current()->getValue() : "EOF");
    while (next() && str != "EOF")
    {
      str.push_back(' ');
      str += current()->getValue();
    }
    
    std::string errorMsg = "Expected token type " + std::to_string(static_cast<int>(expectedType)) +
                           " with value '" + expectedValue + "', but got '" +
                           (current() ? current()->getValue() : "EOF") + "\n" +
                           "AT:" + str;
    throw cbg::ParserError(errorMsg);
}

// Function to consume the current token if it matches the expected type
Token* Parser::mustBe(TokenType expectedType) {
    if (have(expectedType)) {
        Token* temp = new Token(*current());
        next();
        return temp;
    }
    std::string err = (current() ? current()->getValue() : "EOF");
    std::string str = err;
    while (next() && err != "EOF")
    {
      str.push_back(' ');
      str += current()->getValue();
    }
    std::string errorMsg = "Expected token type " + std::to_string(static_cast<int>(expectedType)) +
                           ", but got '" + err + "'" + "\n" +
                           "AT: " + str;;
    throw cbg::ParserError(errorMsg);
}

Token* Parser::lookahead(int offset) {
    size_t tempLineIndex = lineIndex;
    size_t tempTokenIndex = tokenIndex + offset;
    while (tempTokenIndex >= tokenized_code[tempLineIndex].size()) {
        tempLineIndex++;
        if (tempLineIndex >= tokenized_code.size()) {
            return nullptr;
        }
        tempTokenIndex = tempTokenIndex - tokenized_code[tempLineIndex - 1].size();
    }
    return &tokenized_code[tempLineIndex][tempTokenIndex];
}