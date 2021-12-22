#pragma once

#include <iostream>
#include <string>

namespace lox {
namespace parser {

struct Token {
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
    COLON,
    SEMICOLON,
    SLASH,
    STAR,
    BANG,
    EQUAL,
    GREATER,
    LESS,
    QUESTION,

    // double character
    BANG_EQUAL,
    EQUAL_EQUAL,
    GREATER_EQUAL,
    LESS_EQUAL,
    MINUS_EQUAL,
    PLUS_EQUAL,
    SLASH_EQUAL,
    STAR_EQUAL,
    MINUS_MINUS,
    PLUS_PLUS,

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
    // TODO: implement break
    // TODO: implement continue
    END,
  };

  Token(const TokenType type, const int line) : type(type), line(line) {}
  Token(const TokenType type, const std::string& lexeme, const int line)
      : type(type), lexeme(lexeme), line(line){};
  ~Token() {}
  friend std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << "Token[" << static_cast<int>(t.type) << "] " << t.lexeme
       << " [line:" << t.line << "]";
    return os;
  }

  const TokenType type;
  const std::string lexeme;
  const int line;
};

}  // namespace parser
}  // namespace lox