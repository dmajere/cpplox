#pragma once

#include <string_view>
#include <vector>

#include "ControlException.h"
#include "Expression.h"
#include "ParseError.h"
#include "Statement.h"
#include "Token.h"
#include "lox.h"

using TT = lox::parser::Token::TokenType;

constexpr std::string_view kExpectSemicolon = "Expect ';' after expression.";
constexpr std::string_view kExpectLeftParen = "Expect '(' in expression.";
constexpr std::string_view kExpectRightParen = "Expect ')' after expression.";
constexpr std::string_view kExpectLeftBrace = "Expect '{' in expression.";
constexpr std::string_view kExpectRightBrace = "Expect '}' after expression.";
constexpr std::string_view kExpectIdentifier =
    "Expect identifier in expression.";
constexpr std::string_view kExpectExpression = "Expect expression";
constexpr std::string_view kUnexpectedTokenType = "Unexpected token type.";

namespace lox {
namespace parser {

class Parser {
 public:
  Parser(std::vector<Token> tokens) : tokens_(tokens), current_(0) {}

  std::vector<std::shared_ptr<Statement>> parse() {
    std::vector<std::shared_ptr<Statement>> statements;
    while (!isAtEnd()) {
      statements.push_back(declaration());
    }
    return statements;
  }

 private:
  std::shared_ptr<Statement> declaration(bool inLoop = false) {
    try {
      if (match({TT::CLASS})) {
        return classDeclaration();
      }
      if (match({TT::FUN})) {
        return funcDeclaration();
      }
      if (match({TT::VAR})) {
        return varDeclaration();
      }
      return statement(inLoop);
    } catch (ParseError& error) {
      synchronize();
      return nullptr;
    }
  }

  std::shared_ptr<Statement> classDeclaration() {
    Token name = consume(TT::IDENTIFIER, kExpectIdentifier);
    consume(TT::LEFT_BRACE, kExpectLeftBrace);

    std::vector<std::shared_ptr<Function>> methods = {};
    while (!check(TT::RIGHT_BRACE)) {
      methods.push_back(funcDeclaration());
    }
    consume(TT::RIGHT_BRACE, kExpectRightBrace);
    return std::make_shared<Class>(name, std::move(methods));
  }

  std::shared_ptr<Statement> varDeclaration() {
    Token name = consume(TT::IDENTIFIER, kExpectIdentifier);
    std::shared_ptr<Expression> initializer = nullptr;
    if (match({TT::EQUAL})) {
      initializer = expression();
    }
    consume(TT::SEMICOLON, kExpectSemicolon);
    return std::make_shared<Var>(name, std::move(initializer));
  }

  std::shared_ptr<Function> funcDeclaration() {
    Token name = consume(TT::IDENTIFIER, kExpectIdentifier);
    consume(TT::LEFT_PAREN, kExpectLeftParen);

    std::vector<Token> parameters;
    while (!match({TT::RIGHT_PAREN})) {
      if (parameters.size() > 255) {
        error(peek(), "Can't have function with more than 255 parameters.");
      }
      parameters.push_back(consume(TT::IDENTIFIER, kExpectIdentifier));
      match({TT::COMMA});
    }

    consume(TT::LEFT_BRACE, kExpectLeftBrace);
    return std::make_shared<Function>(name, std::move(parameters),
                                      std::make_shared<Block>(block()));
  }

  std::shared_ptr<Statement> statement(bool inLoop = false) {
    if (match({TT::PRINT})) {
      return printStatement();
    }
    if (match({TT::IF})) {
      return ifStatement(inLoop);
    }
    if (match({TT::WHILE})) {
      return whileStatement();
    }
    if (match({TT::FOR})) {
      return forStatement();
    }
    if (match({TT::RETURN})) {
      return returnStatement();
    }
    if (match({TT::CONTINUE})) {
      return continueStatement();
    }
    if (match({TT::BREAK})) {
      return breakStatement();
    }
    if (match({TT::LEFT_BRACE})) {
      return std::make_shared<Block>(block(inLoop));
    }
    return expressionStatement();
  }

  std::shared_ptr<Statement> returnStatement() {
    Token op = previous();
    std::shared_ptr<Expression> value = nullptr;
    if (!check({TT::SEMICOLON})) {
      value = sequence();
    }
    consume(TT::SEMICOLON, kExpectSemicolon);
    return std::make_shared<Return>(op, std::move(value));
  }
  std::shared_ptr<Statement> continueStatement() {
    Token op = previous();
    consume(TT::SEMICOLON, kExpectSemicolon);
    return std::make_shared<Continue>(op);
  }
  std::shared_ptr<Statement> breakStatement() {
    Token op = previous();
    consume(TT::SEMICOLON, kExpectSemicolon);
    return std::make_shared<Break>(op);
  }
  std::shared_ptr<Statement> printStatement() {
    auto value = sequence();
    consume(TT::SEMICOLON, kExpectSemicolon);
    return std::make_shared<Print>(std::move(value));
  }

  std::shared_ptr<Statement> expressionStatement() {
    auto expr = sequence();
    consume(TT::SEMICOLON, kExpectSemicolon);
    return std::make_shared<StatementExpression>(std::move(expr));
  }

  std::vector<std::shared_ptr<Statement>> block(bool inLoop = false) {
    std::vector<std::shared_ptr<Statement>> result;
    while (!check(TT::RIGHT_BRACE) && !isAtEnd()) {
      result.push_back(declaration(inLoop));
    }
    consume(TT::RIGHT_BRACE, kExpectRightBrace);
    return result;
  }

  std::shared_ptr<Statement> ifStatement(bool inLoop = false) {
    consume(TT::LEFT_PAREN, kExpectLeftParen);
    auto condition = sequence();
    consume(TT::RIGHT_PAREN, kExpectRightParen);
    auto then = statement(inLoop);
    if (match({TT::ELSE})) {
      return std::make_shared<If>(std::move(condition), std::move(then),
                                  statement(inLoop));
    }
    return std::make_shared<If>(std::move(condition), std::move(then));
  }

  std::shared_ptr<Statement> whileStatement() {
    consume(TT::LEFT_PAREN, kExpectLeftParen);
    auto condition = sequence();
    consume(TT::RIGHT_PAREN, kExpectRightParen);
    return std::make_shared<While>(std::move(condition), statement(true));
  }

  std::shared_ptr<Statement> forStatement() {
    consume(TT::LEFT_PAREN, kExpectLeftParen);

    std::shared_ptr<Statement> initializer;
    if (match({TT::SEMICOLON})) {
      initializer = nullptr;
    } else if (match({TT::VAR})) {
      initializer = varDeclaration();
    } else {
      initializer = expressionStatement();
    }

    std::shared_ptr<Expression> condition = nullptr;
    if (!check({TT::SEMICOLON})) {
      condition = sequence();
    } else {
      condition = std::make_shared<Literal>(true);
    }
    consume(TT::SEMICOLON, kExpectSemicolon);

    std::shared_ptr<Expression> increment = nullptr;
    if (!check({TT::RIGHT_PAREN})) {
      increment = sequence();
    }
    consume(TT::RIGHT_PAREN, kExpectRightParen);

    auto body = statement(true);
    if (increment) {
      body = std::make_shared<Block>(std::vector<std::shared_ptr<Statement>>{
          std::move(body),
          std::make_shared<StatementExpression>(std::move(increment))});
    }
    body = std::make_shared<While>(std::move(condition), std::move(body));

    if (initializer) {
      body = std::make_shared<Block>(std::vector<std::shared_ptr<Statement>>{
          std::move(initializer), std::move(body)});
    }
    return body;
  }

  std::shared_ptr<Expression> sequence() {
    auto sequence = std::make_shared<Sequence>();
    sequence->expressions.push_back(expression());
    while (match({TT::COMMA})) {
      auto expr = expression();
      sequence->expressions.push_back(std::move(expr));
    }
    return sequence;
  }

  std::shared_ptr<Expression> expression() {
    if (match({TT::LAMBDA})) {
      return lambda();
    }
    return assignment();
  }

  std::shared_ptr<Expression> lambda() {
    Token token = previous();
    consume(TT::LEFT_PAREN, kExpectLeftParen);
    std::vector<Token> parameters;
    while (!match({TT::RIGHT_PAREN})) {
      if (parameters.size() > 255) {
        error(peek(), "Can't have function with more than 255 parameters.");
      }
      parameters.push_back(consume(TT::IDENTIFIER, kExpectIdentifier));
      match({TT::COMMA});
    }

    consume(TT::LEFT_BRACE, kExpectLeftBrace);
    return std::make_shared<Lambda>(std::make_shared<Function>(
        token, std::move(parameters), std::make_shared<Block>(block())));
  }
  std::shared_ptr<Expression> assignment() {
    auto expr = logical_or();

    if (match({TT::EQUAL, TT::PLUS_EQUAL, TT::MINUS_EQUAL, TT::STAR_EQUAL,
               TT::SLASH_EQUAL})) {
      Token equal = previous();
      auto value = assignment();

      if (equal.type != TT::EQUAL) {
        Token::TokenType type;
        switch (equal.type) {
          case TT::PLUS_EQUAL:
            type = TT::PLUS;
            break;
          case TT::MINUS_EQUAL:
            type = TT::MINUS;
            break;
          case TT::STAR_EQUAL:
            type = TT::STAR;
            break;
          case TT::SLASH_EQUAL:
            type = TT::SLASH;
            break;
          default:
            error(equal, kUnexpectedTokenType);
        }
        value = std::make_shared<Binary>(expr, Token(type, equal.line),
                                         std::make_shared<Literal>(1.0));
      }

      if (Variable* e = dynamic_cast<Variable*>(expr.get())) {
        Token name = e->token;
        return std::make_shared<Assignment>(std::move(name), std::move(value));
      }
      error(equal, kExpectExpression);
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
    return call();
  }

  std::shared_ptr<Expression> call() {
    auto func = primary();
    if (match({TT::LEFT_PAREN})) {
      auto args = arguments();
      consume(TT::RIGHT_PAREN, kExpectRightParen);
      func =
          std::make_shared<Call>(std::move(func), previous(), std::move(args));
    }
    return func;
  }

  std::shared_ptr<Expression> arguments() {
    if (check({TT::RIGHT_PAREN})) {
      return nullptr;
    }
    return sequence();
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
      consume(TT::RIGHT_PAREN, kExpectRightParen);
      return std::make_shared<Grouping>(std::move(expr));
    }
    error(peek(), kExpectExpression);
    return nullptr;
  }

  bool match(const std::vector<Token::TokenType>& types) {
    if (check(types)) {
      advance();
      return true;
    }
    return false;
  }
  const Token& consume(Token::TokenType type, const std::string_view& message) {
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

  void error(const Token& token, const std::string_view& message) {
    lox::lang::Lox::error(token, std::string(message));
    throw ParseError(std::string(message));
  }

  std::vector<Token> tokens_;
  int current_;
};

}  // namespace parser
}  // namespace lox