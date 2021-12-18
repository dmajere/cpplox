#include <sstream>
#include <string>
#include <stdexcept>
#include "scanner.h"

namespace lox {
namespace parser {

std::vector<Token> Scanner::scanTokens() {
    std::vector<Token> result;
    while(const char c = peekNextC()) {
        // std::cout << "Next char " << c << "\n";
        if (c == '(') {
            result.push_back(Token(Token::TokenType::LEFT_PAREN, "(", line_));
            getNextC();
        } else if (c == ')') {
            result.push_back(Token(Token::TokenType::RIGHT_PAREN, ")", line_));
            getNextC();
        } else if (c == '{') {
            result.push_back(Token(Token::TokenType::LEFT_BRACE, "{", line_));
            getNextC();
        } else if (c == '}') {
            result.push_back(Token(Token::TokenType::RIGHT_BRACE, "}", line_));
            getNextC();
        } else if (c == ',') {
            result.push_back(Token(Token::TokenType::COMMA, ",", line_));
            getNextC();
        } else if (c == '.') {
            result.push_back(Token(Token::TokenType::DOT, ".", line_));
            getNextC();
        } else if (c == '-') {
            // TODO: implement -=
            result.push_back(Token(Token::TokenType::MINUS, "-", line_));
            getNextC();
        } else if (c == '+') {
            result.push_back(Token(Token::TokenType::PLUS, "+", line_));
            getNextC();
        } else if (c == ';') {
            result.push_back(Token(Token::TokenType::SEMICOLON, ";", line_));
            getNextC();
        } else if (c == '/') {
            result.push_back(Token(Token::TokenType::SLASH, "/", line_));
            getNextC();
        } else if (c == '*') {
            result.push_back(Token(Token::TokenType::STAR, "*", line_));
            getNextC();
        } else if (c == '!') {
            getNextC();
            if (peekNextC() == '=') {
                result.push_back(Token(Token::TokenType::BANG_EQUAL, "!=", line_));
                getNextC();
            } else {
                result.push_back(Token(Token::TokenType::BANG, "!", line_));
            }
        } else if (c == '=') {
            getNextC();
            if (peekNextC() == '=') {
                result.push_back(Token(Token::TokenType::EQUAL_EQUAL, "==", line_));
                getNextC();
            } else {
                result.push_back(Token(Token::TokenType::EQUAL, "=", line_));
            }
        } else if (c == '>') {
            getNextC();
            if (peekNextC() == '=') {
                result.push_back(Token(Token::TokenType::GREATER_EQUAL, ">=", line_));
                getNextC();
            } else {
                result.push_back(Token(Token::TokenType::GREATER, ">", line_));
            }
        } else if (c == '<') {
            getNextC();
            if (peekNextC() == '=') {
                result.push_back(Token(Token::TokenType::LESS_EQUAL, "<=", line_));
                getNextC();
            } else {
                result.push_back(Token(Token::TokenType::LESS, "<", line_));
            }
        } else if (c == '\n') {
            line_++;
            getNextC();
        } else if (c == ' ') {
            getNextC();
        } else if (c == '"') {
            result.push_back(getStringToken());
        } else if (isdigit(c)) {
            result.push_back(getNumberToken());
        } else if (isalpha(c)) {
            result.push_back(getLiteralToken());
        } else {
            // unknown token
            // raise 
            getNextC();
        }
    }
    return result;
}

Token Scanner::getStringToken() {
    getNextC();
    int start = current_pos_;
    while(peekNextC() != '"') {getNextC();}
    getNextC();
    return Token(Token::TokenType::STRING, source_.substr(start + 1, current_pos_ - start - 1), line_);
}

Token::TokenType Scanner::getLiteralTokenType(const std::string& literal) {
    if (literal == "and") {
        return Token::TokenType::AND;
    } else if (literal == "class") {
        return Token::TokenType::CLASS;
    } else if (literal == "else") {
        return Token::TokenType::ELSE;
    } else if (literal == "false") {
        return Token::TokenType::FALSE;
    } else if (literal == "fun") {
        return Token::TokenType::FUN;
    } else if (literal == "for") {
        return Token::TokenType::FOR;
    } else if (literal == "if") {
        return Token::TokenType::IF;
    } else if (literal == "nil") {
        return Token::TokenType::NIL;
    } else if (literal == "or") {
        return Token::TokenType::OR;
    } else if (literal == "print") {
        return Token::TokenType::PRINT;
    } else if (literal == "return") {
        return Token::TokenType::RETURN;
    } else if (literal == "super") {
        return Token::TokenType::SUPER;
    } else if (literal == "this") {
        return Token::TokenType::THIS;
    } else if (literal == "true") {
        return Token::TokenType::TRUE;
    } else if (literal == "var") {
        return Token::TokenType::VAR;
    } else if (literal == "while") {
        return Token::TokenType::WHILE;
    } else {
        return Token::TokenType::IDENTIFIER;
    }
}

Token Scanner::getLiteralToken() {
    getNextC();
    int start = current_pos_;
    while(isalpha(peekNextC()) || isdigit(peekNextC())) {getNextC();}
    const std::string lexem = source_.substr(start, current_pos_ - start + 1);
    return Token(getLiteralTokenType(lexem), std::move(lexem), line_);
}

Token Scanner::getNumberToken() {
    getNextC();
    int start = current_pos_;
    while(isdigit(peekNextC())) {getNextC();}

    if (peekNextC() == '.') {
        //maybe float
        getNextC();
        
        if (char c = peekNextC(); !c || !isdigit(c)) {
            throw "syntax error";
        }
        while(isdigit(peekNextC())) {getNextC();}
    }
    return Token(Token::TokenType::NUMBER, source_.substr(start, current_pos_ - start + 1), line_);
}

const char Scanner::peekNextC() const {
    if (current_pos_ + 1 >= source_.length()) {
        return 0;
    }
    return source_[current_pos_ + 1];
}

const char Scanner::getNextC() {
    auto c = peekNextC();
    current_pos_++;
    return c;
}

}
}