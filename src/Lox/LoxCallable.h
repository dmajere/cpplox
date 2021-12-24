#pragma once
#include <vector>

#include "Interpreter.h"

namespace lox {
namespace lang {

class LoxCallable {
 public:
  virtual std::any call(Interpreter* interpreter,
                        const std::vector<std::any>& args) = 0;
  virtual int arity() const = 0;
  virtual ~LoxCallable() = default;
};

}  // namespace lang
}  // namespace lox