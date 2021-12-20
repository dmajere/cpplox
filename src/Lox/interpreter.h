#pragma once
#include <iostream>
#include <string>
#include "token.h"

namespace lox {
namespace lang {
        
class Interpreter {
    public:
    Interpreter() {}
    ~Interpreter() {}

    void runFromFile(const std::string& path);
    void runPrompt();
    void run(const std::string& code);

    static void error(int line, const std::string& message) {
        report(line, "", message);
    }
    static void error(const lox::parser::Token& tok, const std::string& message) {
        report(tok.line, " at token=" + tok.lexeme, message);
    }

    private:

    static bool hadError;
    
    static void report(int line, const std::string& where, const std::string& message) {
        std::cout << "[line " << line << "] Error " << where << " : " << message << "\n"; 
        hadError = true;
    }

};

} // namespace lang
} // namespace name