#pragma once

#include <any>
#include <vector>

#include "ControlException.h"
#include "Environment.h"
#include "Interpreter.h"
#include "LoxCallable.h"
#include "LoxInstance.h"
#include "RuntimeError.h"
#include "Statement.h"

namespace lox {
namespace lang {
class LoxFunction : public LoxCallable {
 public:
  LoxFunction(const std::shared_ptr<const lox::parser::Function>& declaration,
              std::shared_ptr<Environment> closure)
      : declaration_(std::move(declaration)), closure_(closure) {}

  std::any call(Interpreter* interpreter,
                const std::vector<std::any>& args) override {
    auto env = std::make_shared<Environment>(closure_);
    for (int i = 0; i < declaration_->parameters.size(); i++) {
      auto arg = args[i];
      env->define(declaration_->parameters[i].lexeme, arg);
    }
    try {
      interpreter->evaluate(declaration_->body, env);
    } catch (Return& return_exception) {
      return return_exception.value;
    }

    return nullptr;
  }

  int arity() const override { return declaration_->parameters.size(); }

  std::shared_ptr<LoxFunction> bind(std::shared_ptr<LoxInstance> instance) {
    auto environment = std::make_shared<Environment>(closure_);
    environment->define("this", instance);
    return std::make_shared<LoxFunction>(declaration_, environment);
  }

  std::string toString() const {
    return "Function " + declaration_->name.lexeme;
  }

 private:
  const std::shared_ptr<const lox::parser::Function> declaration_;
  std::shared_ptr<Environment> closure_;
};
}  // namespace lang
}  // namespace lox