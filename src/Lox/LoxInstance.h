#pragma once

#include "LoxClass.h"

namespace lox {
namespace lang {
class LoxClass;

class LoxInstance {
 public:
  LoxInstance(const LoxClass* klass) : klass_{klass} {}

 private:
  const LoxClass* klass_;
};
}  // namespace lang
}  // namespace lox