#pragma once

#include <iostream>
#include <string>

namespace lox {
namespace parser {

class Token {
public:
    enum class TokenType {
        // single character
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        MINUS,
        PLUS,
        SEMICOLON,
        SLASH,
        STAR,

        //double character
        BANG,
        BANG_EQUAL,
        EQUAL,
        EQUAL_EQUAL,
        GREATER,
        GREATER_EQUAL,
        LESS,
        LESS_EQUAL,

        // literals
        IDENTIFIER,
        STRING,
        NUMBER,

        // keywords
        AND,
        CLASS,
        ELSE,
        FALSE,
        FUN,
        FOR,
        IF,
        NIL,
        OR,
        PRINT,
        RETURN,
        SUPER,
        THIS,
        TRUE,
        VAR,
        WHILE,
    };

    Token(const TokenType type, const std::string& lexeme, const int line) : type_(type), lexeme_(lexeme), line_(line) {};
    ~Token() {}
    friend std::ostream& operator<<(std::ostream& os, const Token& t) {
        os << "Token[" << static_cast<int>(t.type_) << "] " << t.lexeme_ << " [line:" << t.line_ << "]";
        return os;
    }

    const TokenType Type() const {
        return type_;
    }

    std::string Lexeme() const {
        return lexeme_;
    }

    const int Line() const {
        return line_;
    }

private:
    const TokenType type_;
    const std::string lexeme_;
    // std::unique_ptr<> literal;
    const int line_;
};

}
}