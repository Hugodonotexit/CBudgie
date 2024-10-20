#include "Parser.h"

// Constructor that takes in file paths
Parser::Parser(std::vector<std::vector<Token>>& tokenized_code) : tokenized_code(tokenized_code) {

    // Initialize token traversal indices
    lineIndex = 0;
    tokenIndex = 0;
}

// The main parsing function
AST* Parser::run() {
    lineIndex = 0;
    tokenIndex = 0;

    // Create the root of the AST
    AST* tree = new AST(TokenType::Program, "");

    // Go through each line and parse the tokens
    while (lineIndex < tokenized_code.size()) {
        tokenIndex = 0;
        while (tokenIndex < tokenized_code[lineIndex].size()) {
            if (have(TokenType::DEF, "def")) {
                tree->addChild(parseFunction());
            } else if (have(TokenType::CLASS, "class")) {
                tree->addChild(parseClass());
            } else if (have(TokenType::VARIABLE)) {
                tree->addChild(parseExpression());
            } else {
                throw cbg::ParserError("Invalid token at global scope: " + current()->getValue());
            }
        }
        lineIndex++;
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
            body->addChild(parseFunction()); // Parse methods
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
    } else {
        throw cbg::ParserError("Invalid statement starting with token: " + current()->getValue());
    }
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
    AST* node = parseLogicalAndExpression();

    while (have(TokenType::OR, "||")) {
        Token* opToken = mustBe(TokenType::OR, "||");
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

    while (have(TokenType::AND, "&&")) {
        Token* opToken = mustBe(TokenType::AND, "&&");
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
        node = opToken;
    }

    return node;
}

// Function to parse multiplicative expressions
AST* Parser::parseMultiplicativeExpression() {
    AST* node = parseUnaryExpression();

    while (have(TokenType::TIMES, "*") || have(TokenType::DIVIDE, "/") || have(TokenType::MODE, "%")) {
        Token* opToken = mustBe(current()->getType()); // '*', '/', '%'
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
        AST* operand = parseUnaryExpression();
        opToken->addChild(operand);
        return opToken;
    } else {
        return parsePrimaryExpression();
    }
}

// Function to parse primary expressions (variables, constants, parenthesized expressions)
AST* Parser::parsePrimaryExpression() {
    if (have(TokenType::NUM_CONST)) {
        // Return the number token directly
        return mustBe(TokenType::NUM_CONST);
    } else if (have(TokenType::WORD_CONST)) {
        // Return the word (string) constant token directly
        return mustBe(TokenType::WORD_CONST);
    } else if (have(TokenType::VARIABLE)) {
        // Check for function calls or variables
        Token* varToken = mustBe(TokenType::VARIABLE);

        if (have(TokenType::L_RBRACKET, "(")) {
            // Function call
            AST* funcCallNode = new AST(TokenType::FunctionCall, "");
            funcCallNode->addChild(varToken); // Function name
            funcCallNode->addChild(parseArgumentList());
            return funcCallNode;
        } else {
            // Variable
            return varToken;
        }
    } else if (have(TokenType::L_RBRACKET, "(")) {
        mustBe(TokenType::L_RBRACKET, "(");
        AST* expr = parseExpression();
        mustBe(TokenType::R_RBRACKET, ")");
        return expr;
    } else {
        throw cbg::ParserError("Expected primary expression, got: " + (current() ? current()->getValue() : "EOF"));
    }
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

    // If we've reached the end of the current line
    if (tokenIndex >= tokenized_code[lineIndex].size()) {
        // Move to the next line if not at the end of tokens
        lineIndex++;
        tokenIndex = 0;

        // Skip empty lines (if any)
        while (lineIndex < tokenized_code.size() && tokenized_code[lineIndex].empty()) {
            lineIndex++;
        }

        // If we've reached the end of tokens
        if (lineIndex >= tokenized_code.size()) {
            return false;
        }
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
        Token* temp = current();
        next();
        return temp;
    }
    std::string errorMsg = "Expected token type " + std::to_string(static_cast<int>(expectedType)) +
                           " with value '" + expectedValue + "', but got '" +
                           (current() ? current()->getValue() : "EOF") + "'";
    throw cbg::ParserError(errorMsg);
}

// Function to consume the current token if it matches the expected type
Token* Parser::mustBe(TokenType expectedType) {
    if (have(expectedType)) {
        Token* temp = current();
        next();
        return temp;
    }
    std::string errorMsg = "Expected token type " + std::to_string(static_cast<int>(expectedType)) +
                           ", but got '" + (current() ? current()->getValue() : "EOF") + "'";
    throw cbg::ParserError(errorMsg);
}