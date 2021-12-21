#pragma once

#include <vector>

#include "expression.h"
#include "lox.h"
#include "token.h"

using TT = lox::parser::Token::TokenType;

namespace lox {
namespace parser {

class Parser {
 public:
  struct ParseError : public std::runtime_error {
    using std::runtime_error::runtime_error;
  };

  Parser(std::vector<Token> tokens) : tokens_(tokens), current_(0) {}

  std::shared_ptr<Expression> parse() {
    try {
      return block();
    } catch (ParseError& e) {
      return nullptr;
    }
  }

 private:
  std::shared_ptr<Expression> block() {
    auto expr = expression();
    if (check(TT::COMMA)) {
      auto block = std::make_shared<Block>();
      block->expressions.push_back(std::move(expr));
      while (match({TT::COMMA})) {
        auto expr = expression();
        block->expressions.push_back(std::move(expr));
      }
      return block;
    }
    return expr;
  }

  std::shared_ptr<Expression> expression(bool inBlockStatement = false) {
    auto expr = equality();

    if (match({TT::QUESTION})) {
      return ternary(std::move(expr));
    }

    return expr;
  }

  std::shared_ptr<Expression> ternary(std::shared_ptr<Expression> predicate) {
    auto then = expression();
    if (match({TT::COLON})) {
      return std::make_shared<Condition>(std::move(predicate), std::move(then),
                                         expression());
    }
    return std::make_shared<Condition>(std::move(predicate), std::move(then));
  }

  std::shared_ptr<Expression> equality() {
    auto expr = comparison();
    while (match({TT::BANG_EQUAL, TT::EQUAL_EQUAL})) {
      Token op = previous();
      auto right = comparison();
      expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::shared_ptr<Expression> comparison() {
    auto expr = term();
    while (match({TT::GREATER, TT::GREATER_EQUAL, TT::LESS, TT::LESS_EQUAL})) {
      Token op = previous();
      auto right = term();
      expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::shared_ptr<Expression> term() {
    auto expr = factor();
    while (match({TT::MINUS, TT::PLUS})) {
      Token op = previous();
      auto right = factor();
      expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::shared_ptr<Expression> factor() {
    auto expr = unary();

    while (match({TT::SLASH, TT::STAR})) {
      Token op = previous();
      auto right = unary();
      expr = std::make_shared<Binary>(std::move(expr), op, std::move(right));
    }
    return expr;
  }

  std::shared_ptr<Expression> unary() {
    if (match({TT::BANG, TT::MINUS})) {
      Token op = previous();
      auto right = unary();
      return std::make_shared<Unary>(op, std::move(right));
    }
    // Prefix increment/decrement
    if (match({TT::PLUS_PLUS, TT::MINUS_MINUS})) {
      Token op = previous();
      auto right = unary();
      return std::make_shared<Unary>(op, std::move(right));
    }
    return primary();
  }

  std::shared_ptr<Expression> primary() {
    if (match({TT::FALSE})) {
      return std::make_shared<Literal>(false);
    }
    if (match({TT::TRUE})) {
      return std::make_shared<Literal>(true);
    }
    if (match({TT::NIL})) {
      return std::make_shared<Literal>(nullptr);
    }
    if (match({TT::NUMBER})) {
      auto number =
          std::make_shared<Literal>(atof(previous().lexeme.c_str()));
      if (match({TT::MINUS_MINUS, TT::PLUS_PLUS})) {
        Token op = previous();
        return std::make_shared<Unary>(op, std::move(number));
      }
      return number;
    }
    if (match({TT::STRING})) {
      return std::make_shared<Literal>(previous().lexeme);
    }
    if (match({TT::LEFT_PAREN})) {
      auto expr = expression();
      consume(TT::RIGHT_PAREN, "Expect ')' after expression.");
      return std::make_shared<Grouping>(std::move(expr));
    }

    lox::lang::Lox::error(peek(), "Expect expresion");
    throw ParseError("Expect expression");
  }

  bool match(const std::vector<Token::TokenType>& types) {
    if (check(types)) {
      advance();
      return true;
    }
    return false;
  }
  const Token& consume(Token::TokenType type, const std::string& message) {
    if (check(type)) return advance();
    lox::lang::Lox::error(peek(), message);
    throw ParseError(message);
  }

  bool check(const Token::TokenType& type) const {
    return isAtEnd() ? false : peek().type == type;
  }
  bool check(const std::vector<Token::TokenType>& types) const {
    for (const Token::TokenType tt : types) {
      if (check(tt)) {
        return true;
      }
    }
    return false;
  }

  const Token& advance() {
    if (!isAtEnd()) current_++;
    return previous();
  }

  bool isAtEnd() const { return peek().type == Token::TokenType::END; }

  const Token& peek() const { return tokens_[current_]; }

  const Token& previous() const { return tokens_[current_ - 1]; }

  void error(const Token& token, const std::string& message) {}

  void synchronize() {
    advance();
    while (!isAtEnd()) {
      if (previous().type == TT::SEMICOLON) return;
      switch (peek().type) {
        case TT::CLASS:
        case TT::FOR:
        case TT::FUN:
        case TT::IF:
        case TT::PRINT:
        case TT::RETURN:
        case TT::VAR:
        case TT::WHILE:
          return;
        default:
          advance();
      }
    }
  }

  std::vector<Token> tokens_;
  int current_;
};

}  // namespace parser
}  // namespace lox