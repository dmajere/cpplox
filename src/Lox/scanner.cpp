#include <sstream>
#include <string>
#include <stdexcept>
#include "scanner.h"
#include "interpreter.h"

namespace lox {
namespace parser {

std::vector<Token> Scanner::scanTokens() {
    std::vector<Token> result;
    while(const char c = peek()) {
        // std::cout << "Next char " << c << "\n";
        if (match('(')) {
            result.push_back(Token(Token::TokenType::LEFT_PAREN, line_));
        } else if (match(')')) {
            result.push_back(Token(Token::TokenType::RIGHT_PAREN, line_));
        } else if (match('{')) {
            result.push_back(Token(Token::TokenType::LEFT_BRACE, line_));
        } else if (match('}')) {
            result.push_back(Token(Token::TokenType::RIGHT_BRACE, line_));
        } else if (match(',')) {
            result.push_back(Token(Token::TokenType::COMMA, line_));
        } else if (match('.')) {
            result.push_back(Token(Token::TokenType::DOT, line_));
        } else if (match('-')) {
            // TODO: implement -=
            result.push_back(Token(Token::TokenType::MINUS, line_));
        } else if (match('+')) {
            result.push_back(Token(Token::TokenType::PLUS, line_));
        } else if (match(';')) {
            result.push_back(Token(Token::TokenType::SEMICOLON, line_));
        } else if (match('/')) {
            if (match('*')) {
                multiLineComment();
            } else if (match('/')) {
                singleLineComment();
            } else {
                result.push_back(Token(Token::TokenType::DIV, line_));
            }
        } else if (match('*')) {
            result.push_back(Token(Token::TokenType::MUL, line_));
        } else if (match('!')) {
            result.push_back(
                Token(
                    match('=') ? 
                        Token::TokenType::BANG_EQUAL : 
                        Token::TokenType::BANG, 
                    line_));
        } else if (match('=')) {
            result.push_back(
                Token(
                    match('=') ? 
                        Token::TokenType::EQUAL_EQUAL : 
                        Token::TokenType::EQUAL, 
                    line_));
        } else if (match('>')) {
            result.push_back(
                Token(
                    match('=') ? 
                        Token::TokenType::GREATER_EQUAL : 
                        Token::TokenType::GREATER, 
                    line_));
        } else if (match('<')) {
            result.push_back(
                Token(
                    match('=') ? 
                        Token::TokenType::LESS_EQUAL : 
                        Token::TokenType::LESS, 
                    line_));
        } else if (match('\n')) {
            line_++;
        } else if (match(' ') || match('\t') || match('\r')) {
            // ignore
        } else if (match('"')) {
            result.push_back(string());
        } else if (isdigit(c)) {
            result.push_back(number());
        } else if (isalpha(c)) {
            result.push_back(identifier());
        } else {
            lox::lang::Interpreter::error(line_, "Unknown charracter: " + std::string{c});
            advance();
        }
    }
    result.push_back(Token(Token::TokenType::END, line_));
    return result;
}

Token Scanner::string() {
    int start = current_pos_;
    while (true) {
        if (isAtTheEnd()) {
            lox::lang::Interpreter::error(line_, "Unterminated string");
            return Token(Token::TokenType::STRING, line_);
        }
        if (match('\\')) {
            //escape character, for simplification we 
            // only support one char escape characters
            advance();
        }
        if (match('"')) {
            break;
        }
        advance();
    }
    return Token(Token::TokenType::STRING, source_.substr(start, current_pos_ - start - 1), line_);
}

void Scanner::multiLineComment() {
    while (true) {
        if (!peek()) {
            lox::lang::Interpreter::error(line_, "Unterminated multiline comment");
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

void Scanner::singleLineComment() {
    while (peek() && peek() != '\n') {advance();}
}

Token::TokenType Scanner::getIdentifierType(const std::string& identifier) {
    try {
        return keywords.at(identifier);
    } catch (std::out_of_range) {
        return Token::TokenType::IDENTIFIER;
    }
}

Token Scanner::identifier() {
    int start = current_pos_;
    while(isalnum(peek())) {advance();}
    const std::string lexem = source_.substr(start, current_pos_ - start);
    return Token(getIdentifierType(lexem), std::move(lexem), line_);
}

Token Scanner::number() {
    int start = current_pos_;
    while(isdigit(peek())) {advance();}

    if (peek() == '.') {
        advance();
        if (isdigit(peek())) {
            while(isdigit(peek())) {advance();}
        } else {
            current_pos_--;
        }
    }
    return Token(Token::TokenType::NUMBER, source_.substr(start, current_pos_ - start), line_);
}

bool Scanner::isAtTheEnd() const {
    return current_pos_ >= source_.length();
}

const char Scanner::peek() const {
    return isAtTheEnd() ? 0 : source_[current_pos_];
}

void Scanner::advance() {
    !isAtTheEnd() && current_pos_++;
}

bool Scanner::match(const char& c) {
    bool result = isAtTheEnd() ? false : peek() == c;
    if (result) {
        advance();
    }
    return result;
}

}
}