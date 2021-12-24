#pragma once
#include "Token.h"

namespace lox {
namespace lang {

struct RuntimeError : public std::runtime_error {
  using std::runtime_error::runtime_error;
  RuntimeError(const lox::parser::Token token, const std::string& message)
      : std::runtime_error(message), token(token) {}
  const lox::parser::Token token;
};

struct ZeroDivision : public RuntimeError {
  ZeroDivision(const lox::parser::Token token, const std::string& message)
      : RuntimeError(token, message) {}
};

}  // namespace lang
}  // namespace lox