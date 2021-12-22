#pragma once

#include <vector>

#include "expression.h"
#include "lox.h"
#include "statement.h"
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

  std::vector<std::shared_ptr<Statement>> parse() {
    std::vector<std::shared_ptr<Statement>> statements;
    while (!isAtEnd()) {
      statements.push_back(declaration());
    }
    return statements;
  }

 private:
  std::shared_ptr<Statement> declaration() {
    try {
      if (match({TT::VAR})) {
        return varDeclaration();
      }
      return statement();
    } catch (ParseError& error) {
      synchronize();
      return nullptr;
    }
  }

  std::shared_ptr<Statement> varDeclaration() {
    Token name = consume(TT::IDENTIFIER, "Expect variable name.");
    std::shared_ptr<Expression> initializer = nullptr;
    if (match({TT::EQUAL})) {
      initializer = expression();
    }
    consume(TT::SEMICOLON, "Expect ';' after expression");
    return std::make_shared<Var>(name, std::move(initializer));
  }

  std::shared_ptr<Statement> statement() {
    if (match({TT::PRINT})) {
      return printStatement();
    }
    if (match({TT::IF})) {
      return ifStatement();
    }
    if (match({TT::WHILE})) {
      return whileStatement();
    }
    if (match({TT::LEFT_BRACE})) {
      return std::make_shared<Block>(block());
    }
    return expressionStatement();
  }

  std::shared_ptr<Statement> printStatement() {
    auto value = sequence();
    consume(TT::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<Print>(std::move(value));
  }

  std::shared_ptr<Statement> expressionStatement() {
    auto expr = sequence();
    consume(TT::SEMICOLON, "Expect ';' after expression.");
    return std::make_shared<StatementExpression>(std::move(expr));
  }

  std::vector<std::shared_ptr<Statement>> block() {
    std::vector<std::shared_ptr<Statement>> result;
    while (!check(TT::RIGHT_BRACE) && !isAtEnd()) {
      result.push_back(declaration());
    }
    consume(TT::RIGHT_BRACE, "Expect '}' after block.");
    return result;
  }

  std::shared_ptr<Statement> ifStatement() {
    consume(TT::LEFT_PAREN, "Expect '(' after if statement.");
    auto condition = sequence();
    consume(TT::RIGHT_PAREN, "Expect ')' after if condition expression."); 
    auto then = statement();
    if (match({TT::ELSE})) {
      return std::make_shared<If>(std::move(condition), std::move(then), statement());
    }
    return std::make_shared<If>(std::move(condition), std::move(then));
  }

  std::shared_ptr<Statement> whileStatement() {
    consume(TT::LEFT_PAREN, "Expect '(' after while statement.");
    auto condition = sequence();
    consume(TT::RIGHT_PAREN, "Expect ')' after while condition expression.");
    return std::make_shared<While>(std::move(condition), statement());
  }

  std::shared_ptr<Expression> sequence() {
    auto expr = expression();
    if (check(TT::COMMA)) {
      auto sequence = std::make_shared<Sequence>();
      sequence->expressions.push_back(std::move(expr));
      while (match({TT::COMMA})) {
        auto expr = expression();
        sequence->expressions.push_back(std::move(expr));
      }
      return sequence;
    }
    return expr;
  }

  std::shared_ptr<Expression> expression() { return assignment(); }

  std::shared_ptr<Expression> assignment() {
    auto expr = logical_or();

    if (match({TT::EQUAL})) {
      //TODO: implement +=, -=, *=, /= by wraping value expr with Binary
      Token equals = previous();
      auto value = assignment();
      if (Variable* e = dynamic_cast<Variable*>(expr.get())) {
        Token name = e->token;
        return std::make_shared<Assignment>(std::move(name), std::move(value));
      }
      error(equals, "Invalid assignment target.");
    }
    if (match({TT::QUESTION})) {
      return ternary(std::move(expr));
    }

    return expr;
  }

  std::shared_ptr<Expression> ternary(std::shared_ptr<Expression> predicate) {
    auto then = expression();
    if (match({TT::COLON})) {
      return std::make_shared<Ternary>(std::move(predicate), std::move(then),
                                         expression());
    }
    return std::make_shared<Ternary>(std::move(predicate), std::move(then));
  }

  std::shared_ptr<Expression> logical_or() {
    auto expr = logical_and();
    if (match({TT::OR})) {
      Token op = previous();
      expr = std::make_shared<Binary>(std::move(expr), op, logical_and());
    }
    return expr;
  }

  std::shared_ptr<Expression> logical_and() {
    auto expr = equality();
    if (match({TT::AND})) {
      Token op = previous();
      expr = std::make_shared<Binary>(std::move(expr), op, equality());
    }
    return expr;
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
    if (match({TT::IDENTIFIER})) {
      auto identifier = std::make_shared<Variable>(previous());
      if (match({TT::MINUS_MINUS, TT::PLUS_PLUS})) {
        Token op = previous();
        return std::make_shared<Unary>(op, std::move(identifier));
      }
      return identifier;
    }
    if (match({TT::NUMBER})) {
      auto number = std::make_shared<Literal>(atof(previous().lexeme.c_str()));
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
    error(peek(), "Expect expresion");
    return nullptr;
  }

  bool match(const std::vector<Token::TokenType>& types) {
    if (check(types)) {
      advance();
      return true;
    }
    return false;
  }
  const Token& consume(Token::TokenType type, const std::string& message) {
    if (!check(type)) {
      error(peek(), message);
    }
    return advance();
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

  void error(const Token& token, const std::string& message) {
    lox::lang::Lox::error(token, message);
    throw ParseError(message);
  }

  std::vector<Token> tokens_;
  int current_;
};

}  // namespace parser
}  // namespace lox