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
}

bool isCompleteStatement(const std::vector<lox::parser::Token>& tokens) {
  int openBraces = 0;
  int openParens = 0;

  for (const auto& token : tokens) {
    if (token.type == lox::parser::Token::TokenType::LEFT_BRACE) {
      openBraces++;
    } else if (token.type == lox::parser::Token::TokenType::RIGHT_BRACE) {
      openBraces--;
    } else if (token.type == lox::parser::Token::TokenType::LEFT_PAREN) {
      openParens++;
    } else if (token.type == lox::parser::Token::TokenType::RIGHT_PAREN) {
      openParens--;
    }
  }
  return openParens == 0 && openBraces == 0;
}

void Lox::runPrompt() {
  auto printer = std::make_unique<lox::parser::AstPrinter>();
  auto interpreter = std::make_shared<lox::lang::Interpreter>();
  auto resolver = std::make_unique<lox::lang::Resolver>(interpreter);
  std::vector<lox::parser::Token> tokens;

  // std::cout << "\n" << kLoxInputPrompt;
  for (std::string line;; std::getline(std::cin, line)) {
    hadError = false;

    if (std::cin.fail()) {
      return;
    }

    if (!line.empty()) {
      auto scanner = lox::parser::Scanner(line);
      if (!tokens.empty()) {
        tokens.pop_back();
      }

      auto new_tokens = scanner.scanTokens();
      tokens.reserve(tokens.size() + new_tokens.size());
      for (auto token : new_tokens) {
        tokens.push_back(std::move(token));
      }

      if (!isCompleteStatement(tokens)) {
        std::cout << "...";
        continue;
      }
      // for (const auto& token : tokens) {
      //   std::cout << "'" << token.lexeme << "'\n";
      // }
      auto parser = lox::parser::Parser(tokens);
      auto statements = parser.parse();

      if (hadError) {
        tokens.clear();
        std::cout << kLoxInputPrompt;
        continue;
      };

      // std::cout << "=== AST ===\n";
      // for (const auto& stmt : statements) {
      //   if (stmt) {
      //     std::cout << printer->print(stmt) << "\n";
      //   }
      // }
      // std::cout << "=== === ===\n";

      try {
        // std::cout << "=== Resolve ===\n";
        resolver->resolve(statements);
        // std::cout << "=== ======= ===\n";
        if (hadError) {
          tokens.clear();
          std::cout << kLoxInputPrompt;
          continue;
        }

        // std::cout << "=== Interpret ===\n";
        interpreter->evaluate(statements);
        // std::cout << "=== ========= ===\n";
        tokens.clear();
      } catch (RuntimeError& error) {
        std::cout << "Error: " << error.what();
        tokens.clear();
      }
    }

    if (!tokens.empty()) {
      std::cout << "...";
    } else {
      std::cout << kLoxInputPrompt;
    }
  }
}

void Lox::run(const std::string& source) {}

}  // namespace lang
}  // namespace lox