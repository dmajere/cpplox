#pragma once
#include <vector>
#include <unordered_map>
#include "token.h"

namespace lox {
namespace parser {

const std::unordered_map<std::string, Token::TokenType> keywords = {
    {"and", Token::TokenType::AND},
    {"class", Token::TokenType::CLASS},
    {"else", Token::TokenType::ELSE},
    {"false", Token::TokenType::FALSE},
    {"fun", Token::TokenType::FUN},
    {"for", Token::TokenType::FOR},
    {"if", Token::TokenType::IF},
    {"nil", Token::TokenType::NIL},
    {"or", Token::TokenType::OR},
    {"print", Token::TokenType::PRINT},
    {"return", Token::TokenType::RETURN},
    {"super", Token::TokenType::SUPER},
    {"this", Token::TokenType::THIS},
    {"true", Token::TokenType::TRUE},
    {"var", Token::TokenType::VAR},
    {"while", Token::TokenType::WHILE},
};

class Scanner
{
public:
    Scanner(const std::string& source): source_(source), line_(1), current_pos_(0) {}; 
    ~Scanner() {};
    std::vector<Token> scanTokens(); 

private:
    const std::string source_;
    int line_;
    int current_pos_;

    Token number();
    Token identifier();
    Token string();
    void multiLineComment();
    void singleLineComment();
    Token::TokenType getIdentifierType(const std::string& literal);
    bool match(const char& c);
    void advance();
    const char peek() const;
    bool isAtTheEnd() const;

};

}
}