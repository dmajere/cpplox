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

  for (std::string line;; std::getline(std::cin, line)) {
    if (std::cin.fail()) {
      return;
    }
    if (!line.empty()) {
      auto scanner = lox::parser::Scanner(line);
      auto tokens = scanner.scanTokens();
      auto parser = lox::parser::Parser(tokens);
      auto statements = parser.parse();
      interpreter.evaluate(statements);

      std::cout << "\n";
    }
    std::cout << kLoxInputPrompt;
  }
}

void Lox::run(const std::string& source) {
  auto printer = lox::parser::AstPrinter();
}

}  // namespace lang
}  // namespace lox