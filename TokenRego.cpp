#include "TokenRego.h"

TokenRego::TokenRego()
{
    // Keywords
    add("import", Token::IMPORT);
    add("def", Token::DEF);
    add("class", Token::CLASS);
    add("read", Token::READ);
    add("print", Token::PRINT);
    add("if", Token::IF);
    add("else", Token::ELSE);
    add("do", Token::DO);
    add("while", Token::WHILE);
    add("for", Token::FOR);
    add("true", Token::TRUE);
    add("false", Token::FALSE);
    add("break", Token::BREAK);
    add("continue", Token::CONTINUE);
    add("return", Token::RETURN);
    add("and", Token::AND);
    add("or", Token::OR);
    add("not", Token::NOT);
    add("xor", Token::XOR);
    add("numeric", Token::NUMERIC);

    // Operators
    add("+", Token::PLUS);
    add("-", Token::MINUS);
    add("*", Token::TIMES);
    add("/", Token::DIVIDE);
    add("%", Token::MODE);
    add("**", Token::POW);
    add("=", Token::EQUAL);
    add(">", Token::GREATER);
    add("<", Token::SMALLER);
    add(">=", Token::E_GREATER);
    add("<=", Token::E_SMALLER);
    add("==", Token::EQUIVALENT);
    add("!=", Token::NOT_EQUIVALENT);

    // Punctuation
    add(",", Token::COMMA);
    add(":", Token::COLON);
    add(".", Token::DOT);
    add("(", Token::L_RBRACKET);
    add(")", Token::R_RBRACKET);
    add("[", Token::L_SQBRACKET);
    add("]", Token::R_SQBRACKET);
    add("{", Token::L_SBRACKET);
    add("}", Token::R_SBRACKET);

    // Built-in conversions
    add("str", Token::TO_STRING);
    add("bool", Token::TO_BOOL);
    add("num", Token::TO_NUM);
}

void TokenRego::add(const std::string& text, Token token)
{
    text_to_token_[text] = token;

    const auto id = static_cast<size_t>(token);
    if (token_to_text_.size() <= id)
        token_to_text_.resize(id + 1);

    token_to_text_[id] = text;
}

std::optional<TokenRego::Token>
TokenRego::lookup(std::string_view text) const
{
    auto it = text_to_token_.find(std::string(text));
    if (it == text_to_token_.end())
        return std::nullopt;
    return it->second;
}

std::optional<std::string_view>
TokenRego::spelling(Token token) const
{
    const auto id = static_cast<size_t>(token);
    if (id >= token_to_text_.size())
        return std::nullopt;

    const auto& s = token_to_text_[id];
    if (s.empty())
        return std::nullopt;

    return s;
}
