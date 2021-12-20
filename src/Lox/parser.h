#pragma once
#include <vector>
#include <Folly/Conv.h>
#include "token.h"
#include "expression.h"
#include "interpreter.h"

using TT = lox::parser::Token::TokenType;

namespace lox {
namespace parser {


class Parser {
    public:
    Parser(std::vector<Token> tokens) : tokens_(tokens), current_(0) {}

    std::unique_ptr<Expression> parse() {
        try {
            return expression();
        } catch(std::domain_error& e) {
            return nullptr;
        }
    }

    private:

    std::unique_ptr<Expression> expression() {
        return equality();
    }

    std::unique_ptr<Expression> equality() {
        auto expr = comparison();
        while (match({TT::BANG_EQUAL, TT::EQUAL_EQUAL})) {
            Token op = previous();
            auto right = comparison();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expression> comparison() {
        auto expr = term();
        while(match({TT::GREATER, TT::GREATER_EQUAL, TT::LESS, TT::LESS_EQUAL})) {
            Token op = previous();
            auto right = term();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expression> term() {
        auto expr = factor();
        while(match({TT::MINUS, TT::PLUS})) {
            Token op = previous();
            auto right = factor();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expression> factor() {
        auto expr = unary();

        while(match({TT::DIV, TT::MUL})){
            Token op = previous();
            auto right = unary();
            expr = std::make_unique<Binary>(std::move(expr), op, std::move(right));
        }
        return expr;
    }

    std::unique_ptr<Expression> unary() {
        if(match({TT::BANG, TT::MINUS})) {
            Token op = previous();
            auto right = unary();
            return std::make_unique<Unary>(op, std::move(right));
        }
        return primary();
    }

    std::unique_ptr<Expression> primary() {
        if(match({TT::FALSE})) {
            return std::make_unique<Literal>(false);
        }
        if(match({TT::TRUE})) {
            return std::make_unique<Literal>(true);
        }
        if (match({TT::NIL})) {
            return nullptr;
        }
        if (match({TT::NUMBER})) {
            return std::make_unique<Literal>(folly::to<double>(previous().Lexeme()));
        }
        if (match({TT::STRING})) {
            return std::make_unique<Literal>(previous().Lexeme());
        }
        if (match({TT::LEFT_PAREN})) {
            auto expr = expression();
            consume(TT::RIGHT_PAREN, "Expect ')' after expression.");
            return std::make_unique<Grouping>(std::move(expr));
        }

        lox::lang::Interpreter::error(peek(), "Expect expresion");
        throw std::domain_error("Expect expression");
    }

    bool match(std::vector<Token::TokenType> types) {
        for (const auto& t : types) {
            if (check(t)) {
                advance();
                return true;
            }
        }
        return false;
    }
    const Token& consume(Token::TokenType type, const std::string& message) {
        if (check(type)) return advance();
        lox::lang::Interpreter::error(peek(), message);
        throw std::domain_error(message);
    }

    bool check(const Token::TokenType& type) const {
        return isAtEnd() ? false : peek().Type() == type;
    }

    const Token& advance() {
        !isAtEnd() && current_++;
        return previous();
    }

    bool isAtEnd() const {
        return peek().Type() == Token::TokenType::END;
    }

    const Token& peek() const {
        return tokens_[current_];
    }

    const Token& previous() const {
        return tokens_[current_ - 1];
    }

    void error(const Token& token, const std::string& message) {
    }

    void synchronize() {
        advance();
        while(!isAtEnd()) {
            if (previous().Type() == TT::SEMICOLON) return;
            switch(peek().Type()) {
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

}
}