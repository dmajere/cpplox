#include "lox.h"

#include <folly/File.h>
#include <folly/FileUtil.h>

#include <iostream>
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
  auto interpreter = lox::lang::Interpreter();
  auto printer = lox::parser::AstPrinter();

  for (std::string line;; std::getline(std::cin, line)) {
    if (std::cin.fail()) {
      return;
    }
    if (!line.empty()) {
      auto scanner = lox::parser::Scanner(line);
      auto tokens = scanner.scanTokens();
      // for (const auto& tok : tokens) {
      //   std::cout << "Token: " << tok << "\n";
      // }
      auto parser = lox::parser::Parser(tokens);
      auto statements = parser.parse();
      for (const auto& stmt : statements) {
        std::cout << "AST: " << printer.print(stmt) << "\n";
      }

      try {
        interpreter.evaluate(statements);
      } catch (RuntimeError& error) {
        std::cout << "Error: " << error.what();
      }

      std::cout << "\n";
    }
    std::cout << kLoxInputPrompt;
  }
}

void Lox::run(const std::string& source) {}

}  // namespace lang
}  // namespace lox