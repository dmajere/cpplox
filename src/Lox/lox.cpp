#include "lox.h"

#include <folly/File.h>
#include <folly/FileUtil.h>

#include <iostream>
#include <string_view>

#include "AstPrinter.h"
#include "Interpreter.h"
#include "Parser.h"
#include "Resolver.h"
#include "Scanner.h"

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
  auto printer = std::make_unique<lox::parser::AstPrinter>();
  auto interpreter = std::make_shared<lox::lang::Interpreter>();
  auto resolver = std::make_unique<lox::lang::Resolver>(interpreter);

  for (std::string line;; std::getline(std::cin, line)) {
    hadError = false;
    if (std::cin.fail()) {
      return;
    }
    if (!line.empty()) {
      auto scanner = lox::parser::Scanner(line);
      auto tokens = scanner.scanTokens();
      auto parser = lox::parser::Parser(tokens);
      auto statements = parser.parse();

      if (hadError) continue;
      std::cout << "=== AST ===\n";
      for (const auto& stmt : statements) {
        if (stmt) {
          std::cout << printer->print(stmt) << "\n";
        }
      }
      std::cout << "=== === ===\n";

      try {
        std::cout << "=== Resolve ===\n";
        resolver->resolve(statements);
        std::cout << "=== ======= ===\n";
        if (hadError) continue;

        std::cout << "=== Interpret ===\n";
        interpreter->evaluate(statements);
        std::cout << "=== ========= ===\n";
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