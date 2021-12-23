#pragma once
#include <stdexcept>

#include "expression.h"

namespace lox {
namespace lang {

struct ControlException : public std::runtime_error {
  using std::runtime_error::runtime_error;
  ControlException() : std::runtime_error(""){};
};

struct Continue : public ControlException {
  Continue() : ControlException(){};
};

struct Break : public ControlException {
  Break() : ControlException(){};
};

struct Return : public ControlException {
  Return(const std::shared_ptr<lox::parser::Expression>& value)
      : ControlException(), value(value) {}

  const std::shared_ptr<lox::parser::Expression> value;
};

}  // namespace lang
}  // namespace lox