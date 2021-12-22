#pragma once
#include <any>
#include <iostream>
#include <string>

#include "RuntimeError.h"
#include "interpreter.h"
#include "token.h"

namespace lox {
namespace lang {

class Lox {
 public:
  Lox() {}
  ~Lox() {}

  void runFromFile(const std::string& path);
  void runPrompt();
  void run(const std::string& code);

  static void error(int line, const std::string& message) {
    report(line, "", message);
  }

  static void error(const lox::parser::Token& tok, const std::string& message) {
    report(tok.line, " at token " + tok.lexeme, message);
  }

  static void runtime_error(lox::lang::RuntimeError& error) {
    report(error.token.line, "at token " + error.token.lexeme, error.what());
    hadRuntimeError = true;
  }

 private:
  static bool hadError;
  static bool hadRuntimeError;

  static void report(int line, const std::string& where,
                     const std::string& message) {
    std::cout << "[line " << line << "] Error " << where << " : " << message
              << "\n";
    hadError = true;
  }

  static std::string print_output(const std::any& object) {
    auto& object_type = object.type();

    if (!object.has_value()) {
      return "nil";
    }
    if (object_type == typeid(nullptr)) {
      return "nil";
    }
    if (object_type == typeid(std::string)) {
      return "\"" + std::any_cast<std::string>(object) + "\"";
    }
    if (object_type == typeid(bool)) {
      return std::any_cast<bool>(object) ? "true" : "false";
    }
    if (object_type == typeid(double)) {
      return std::to_string(std::any_cast<double>(object));
    }
    return "nil";
  }
};

}  // namespace lang
}  // namespace lox