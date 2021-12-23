#pragma once

#include <chrono>

#include "Callable.h"

namespace lox {
namespace lang {

class Clock : public LoxCallable {
    public:
  std::any call(Interpreter* interpreter, const std::vector<std::any>& args) override {
    return std::chrono::duration<double>(std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::system_clock::now().time_since_epoch())).count();
  }
  int arity() const override { return 0; }
};
}  // namespace lang

}  // namespace lox