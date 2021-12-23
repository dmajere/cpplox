#pragma once

#include <any>
#include <vector>

#include "Callable.h"
#include "ControlException.h"
#include "RuntimeError.h"
#include "environment.h"
#include "interpreter.h"
#include "statement.h"

namespace lox {
namespace lang {
class LoxFunction : public LoxCallable {
 public:
  LoxFunction(const std::shared_ptr<const lox::parser::Function>& declaration)
      : declaration_(std::move(declaration)) {}
  std::any call(Interpreter* interpreter,
                const std::vector<std::any>& args) override {
    auto env = std::make_shared<Environment>(interpreter->environment());
    for (int i = 0; i < declaration_->parameters.size(); i++) {
      auto arg = args[i];
      env->define(declaration_->parameters[i].lexeme, arg);
    }
    try {
      interpreter->evaluate(declaration_->body, env);
    } catch (std::shared_ptr<lox::lang::ControlException>& exception) {
      if (lox::lang::Return* ret =
              dynamic_cast<lox::lang::Return*>(exception.get())) {
        return interpreter->evaluate(ret->value);
      }
      throw exception;
    }
    return nullptr;
  }
  int arity() const override { return declaration_->parameters.size(); }

 private:
  const std::shared_ptr<const lox::parser::Function> declaration_;
};
}  // namespace lang
}  // namespace lox