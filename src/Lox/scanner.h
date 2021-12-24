#pragma once
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "Token.h"
#include "lox.h"

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
    {"break", Token::TokenType::BREAK},
    {"continue", Token::TokenType::CONTINUE},
    {"lambda", Token::TokenType::LAMBDA},
};

class Scanner {
 public:
  Scanner(const std::string& source)
      : source_(source), line_(1), current_pos_(0){};
  std::vector<Token> scanTokens() {
    std::vector<Token> result;
    while (const char c = peek()) {
      // Single character tokens
      if (match('(')) {
        result.push_back(Token(Token::TokenType::LEFT_PAREN, "(", line_));
      } else if (match(')')) {
        result.push_back(Token(Token::TokenType::RIGHT_PAREN, ")", line_));
      } else if (match('{')) {
        result.push_back(Token(Token::TokenType::LEFT_BRACE, "{", line_));
      } else if (match('}')) {
        result.push_back(Token(Token::TokenType::RIGHT_BRACE, "}", line_));
      } else if (match(',')) {
        result.push_back(Token(Token::TokenType::COMMA, ",", line_));
      } else if (match('.')) {
        result.push_back(Token(Token::TokenType::DOT, ".", line_));
      } else if (match('?')) {
        result.push_back(Token(Token::TokenType::QUESTION, "?", line_));
      } else if (match(':')) {
        result.push_back(Token(Token::TokenType::COLON, ":", line_));
      } else if (match(';')) {
        result.push_back(Token(Token::TokenType::SEMICOLON, ";", line_));
      } else if (match('\n')) {
        line_++;
      } else if (match(' ') || match('\t') || match('\r')) {
        // ignore spaces, tabs and carry
        // Single and Double character tokens
      } else if (match('-')) {
        if (match('-')) {
          result.push_back(Token(Token::TokenType::MINUS_MINUS, "--", line_));
        } else if (match('=')) {
          result.push_back(Token(Token::TokenType::MINUS_EQUAL, "-=", line_));
        } else {
          result.push_back(Token(Token::TokenType::MINUS, "+", line_));
        }
      } else if (match('+')) {
        if (match('+')) {
          result.push_back(Token(Token::TokenType::PLUS_PLUS, "++", line_));
        } else if (match('=')) {
          result.push_back(Token(Token::TokenType::PLUS_EQUAL, "+=", line_));
        } else {
          result.push_back(Token(Token::TokenType::PLUS, "+", line_));
        }
      } else if (match('/')) {
        if (match('*')) {
          multiLineComment();
        } else if (match('/')) {
          singleLineComment();
        } else if (match('=')) {
          result.push_back(Token(Token::TokenType::SLASH_EQUAL, "/=", line_));
        } else {
          result.push_back(Token(Token::TokenType::SLASH, "/", line_));
        }
      } else if (match('*')) {
        if (match('=')) {
          result.push_back(Token(Token::TokenType::STAR_EQUAL, "*=", line_));
        } else {
          result.push_back(Token(Token::TokenType::STAR, "*", line_));
        }
      } else if (match('!')) {
        if (match('=')) {
          result.push_back(Token(Token::TokenType::BANG_EQUAL, "!=", line_));

        } else {
          result.push_back(Token(Token::TokenType::BANG, "!", line_));
        }
      } else if (match('=')) {
        if (match('=')) {
          result.push_back(Token(Token::TokenType::EQUAL_EQUAL, "==", line_));
        } else {
          result.push_back(Token(Token::TokenType::EQUAL, "=", line_));
        }
      } else if (match('>')) {
        if (match('=')) {
          result.push_back(Token(Token::TokenType::GREATER_EQUAL, ">=", line_));
        } else {
          result.push_back(Token(Token::TokenType::GREATER, ">", line_));
        }
      } else if (match('<')) {
        if (match('=')) {
          result.push_back(Token(Token::TokenType::LESS_EQUAL, "<=", line_));
        } else {
          result.push_back(Token(Token::TokenType::LESS, "<", line_));
        }
      } else if (match('"')) {
        // string
        result.push_back(string());
      } else if (isdigit(c)) {
        // number
        result.push_back(number());
      } else if (isalpha(c)) {
        // identifiers
        result.push_back(identifier());
      } else {
        lox::lang::Lox::error(line_, "Unknown charracter: " + std::string{c});
        advance();
      }
    }
    result.push_back(Token(Token::TokenType::END, line_));
    return result;
  }

 private:
  const std::string source_;
  int line_;
  int current_pos_;

  void multiLineComment() {
    while (true) {
      if (!peek()) {
        lox::lang::Lox::error(line_, "Unterminated multiline comment");
        break;
      }
      if (peek() != '*') {
        advance();
      } else {
        advance();
        if (peek() == '/') {
          advance();
          break;
        }
      }
    }
  }
  void singleLineComment() {
    while (peek() && peek() != '\n') {
      advance();
    }
  }
  Token string() {
    int start = current_pos_;
    while (true) {
      if (isAtTheEnd()) {
        lox::lang::Lox::error(line_, "Unterminated string");
        return Token(Token::TokenType::STRING, line_);
      }
      if (match('\\')) {
        // escape character, for simplification we
        //  only support one char escape characters
        advance();
      }
      if (match('"')) {
        break;
      }
      advance();
    }
    return Token(Token::TokenType::STRING,
                 source_.substr(start, current_pos_ - start - 1), line_);
  }
  Token number() {
    int start = current_pos_;
    while (isdigit(peek())) {
      advance();
    }

    if (peek() == '.') {
      advance();
      if (isdigit(peek())) {
        while (isdigit(peek())) {
          advance();
        }
      } else {
        current_pos_--;
      }
    }
    return Token(Token::TokenType::NUMBER,
                 source_.substr(start, current_pos_ - start), line_);
  }
  Token identifier() {
    int start = current_pos_;
    while (isalnum(peek())) {
      advance();
    }
    auto identifier = source_.substr(start, current_pos_ - start);
    Token::TokenType type;
    try {
      type = keywords.at(identifier);
    } catch (std::out_of_range) {
      type = Token::TokenType::IDENTIFIER;
    }
    return Token(type, std::move(identifier), line_);
  }

  bool match(const char& c) {
    bool result = isAtTheEnd() ? false : peek() == c;
    if (result) {
      advance();
    }
    return result;
  }
  void advance() {
    if (!isAtTheEnd()) current_pos_++;
  }
  const char peek() const { return isAtTheEnd() ? 0 : source_[current_pos_]; }
  bool isAtTheEnd() const { return current_pos_ >= source_.length(); }
};

}  // namespace parser
}  // namespace lox