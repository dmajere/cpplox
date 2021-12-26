#pragma once
#include <any>
#include <string>

#include "Interpreter.h"
#include "LoxCallable.h"
#include "LoxInstance.h"

namespace lox {
namespace lang {
class LoxClass : public LoxCallable {
 public:
  LoxClass(const std::string& name) : name_{name} {}

  std::string Name() const { return name_; }
  std::any call(Interpreter* interpreter,
                const std::vector<std::any>& args) override {
    return std::make_any<std::shared_ptr<LoxInstance>>(
        std::make_shared<LoxInstance>(this));
  }
  int arity() const override { return 0; }

 private:
  const std::string name_;
};
}  // namespace lang
}  // namespace lox