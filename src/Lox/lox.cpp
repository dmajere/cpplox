#include "lox.h"
#include <iostream>
#include <folly/File.h>
#include <folly/FileUtil.h>

#include <string_view>

#include "AstPrinter.h"
#include "parser.h"
#include "scanner.h"

constexpr std::string_view kLoxInputPrompt = "[In]: ";
constexpr std::string_view kLoxOutputPrompt = "[Out]: ";

namespace lox {
namespace lang {

bool Lox::hadError = false;
bool Lox::hadRuntimeError = false;


void Lox::runFromFile(const std::string& path) {
  auto f = folly::File(path);
  std::string code;
  folly::readFile(f.fd(), code);
  run(code);
  // TODO: had error exits
}

void Lox::runPrompt() {
  for (std::string line;; std::getline(std::cin, line)) {
    if (std::cin.fail()) {
      return;
    }
    if (!line.empty()) {
      run(line);
      std::cout << "\n";
    }
    std::cout << kLoxInputPrompt;
  }
}

void Lox::run(const std::string& source) {
  auto scanner = lox::parser::Scanner(source);
  auto tokens = scanner.scanTokens();
  //if debug
  for (const lox::parser::Token& t : tokens) {
    std::cout << t << "\n";
  }

  auto parser = lox::parser::Parser(tokens);
  auto expr = parser.parse();

  if (expr) {
    auto printer = lox::parser::AstPrinter();
    //TODO if debug
    std::cout << "AST: " << printer.print(expr) << "\n";

    auto interpreter = lox::lang::Interpreter();
    try {
      auto result = interpreter.evaluate(expr);
      std::cout << kLoxOutputPrompt << lox::lang::Interpreter::toString(result);  
    } catch (lox::lang::Interpreter::RuntimeError& e) {
      runtime_error(e);
    }
  }
}


}  // namespace lang
}  // namespace lox