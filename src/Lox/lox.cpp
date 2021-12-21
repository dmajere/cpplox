#include "lox.h"
#include <iostream>
#include <folly/File.h>
#include <folly/FileUtil.h>

#include <string_view>

#include "AstPrinter.h"
#include "parser.h"
#include "scanner.h"
#include "interpreter.h"

constexpr std::string_view kLoxInputPrompt = "[in]=> ";
constexpr std::string_view kLoxOutputPrompt = "[out]: ";

namespace lox {
namespace lang {

bool Lox::hadError = false;


void Lox::runFromFile(const std::string& path) {
  auto f = folly::File(path);
  std::string code;
  folly::readFile(f.fd(), code);
  run(code);
}

void Lox::runPrompt() {
  for (std::string line;; std::getline(std::cin, line)) {
    if (std::cin.fail()) {
      return;
    }
    // TODO: result is expression
    // print output prompt + expression.repr()
    if (!line.empty()) {
      run(line);
      std::cout << "\n";
    }
    if (hadError) {
      return;
    }
    std::cout << kLoxInputPrompt;
  }
}

void Lox::run(const std::string& source) {
  auto scanner = lox::parser::Scanner(source);
  auto tokens = scanner.scanTokens();

  for (const lox::parser::Token& t : tokens) {
    std::cout << t << "\n";
  }

  auto parser = lox::parser::Parser(tokens);
  auto expr = parser.parse();

  auto printer = lox::parser::AstPrinter();
  std::cout << "AST: " << printer.print(expr) << "\n";

  auto interpreter = lox::lang::Interpreter();
  auto result = interpreter.evaluate(expr);
  std::cout << kLoxOutputPrompt << print_output(result);  
}


}  // namespace lang
}  // namespace lox