#pragma once
#include <any>
#include <string>
#include <unordered_map>

#include "RuntimeError.h"
#include "interpreter.h"
#include "token.h"

namespace lox {
namespace lang {

class Environment {
 public:
  Environment() : parent_(nullptr) {}
  Environment(std::shared_ptr<Environment> parent) : parent_(parent) {}
  ~Environment() = default;

  void define(const std::string& name, std::any& value) {
    values_.insert({name, value});
  }

  std::any get(const lox::parser::Token& name) const {
    auto it = values_.find(name.lexeme);
    if (it == values_.end()) {
      throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
    }
    return it->second;
  }

 private:
  std::unordered_map<std::string, std::any> values_;
  const std::shared_ptr<Environment> parent_;
};

}  // namespace lang
}  // namespace lox