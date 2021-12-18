#pragma once
#include <vector>
#include "token.h"

namespace lox {
namespace parser {

class Scanner
{
public:
    Scanner(const std::string& source): source_(source), line_(1), current_pos_(-1) {}; 
    ~Scanner() {};
    std::vector<Token> scanTokens(); 

private:
    const std::string source_;
    int line_;
    int current_pos_;

    Token getNumberToken();
    Token getLiteralToken();
    Token getStringToken();
    Token::TokenType getLiteralTokenType(const std::string& literal);
    const char getNextC();
    const char peekNextC() const;

};

}
}