#pragma once
#include <any>
#include <stdexcept>

#include "Expression.h"
#include "Token.h"

namespace lox {
namespace lang {

struct ControlException : public std::runtime_error {
  using std::runtime_error::runtime_error;
  explicit ControlException(const lox::parser::Token& token)
      : std::runtime_error(""), token(token){};

  const lox::parser::Token token;
};

struct Continue : public ControlException {
  explicit Continue(const lox::parser::Token& token)
      : ControlException(token){};
};

struct Break : public ControlException {
  explicit Break(const lox::parser::Token& token) : ControlException(token){};
};

struct Return : public ControlException {
  Return(const lox::parser::Token& token, const std::any& value)
      : ControlException(token), value(value) {}

  const std::any value;
};

}  // namespace lang
}  // namespace lox