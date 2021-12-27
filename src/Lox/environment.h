#pragma once
#include <any>
#include <string>
#include <unordered_map>

#include "Interpreter.h"
#include "RuntimeError.h"
#include "Token.h"

namespace lox {
namespace lang {

class Environment {
 public:
  Environment() : parent_(nullptr) {}
  Environment(std::shared_ptr<Environment> parent) : parent_(parent) {}
  ~Environment() = default;

  void define(const std::string& name, std::any value) {
    values_.insert({name, value});
  }

  void define(const lox::parser::Token& name, std::any& value) {
    values_.insert({name.lexeme, value});
  }

  void assign(const lox::parser::Token& name, std::any& value) {
    auto it = values_.find(name.lexeme);
    if (it != values_.end()) {
      it->second = value;
      return;
    }
    if (parent_) {
      parent_->assign(name, value);
      return;
    }
    throw RuntimeError(name,
                       "Assinment to unbound variable '" + name.lexeme + "'.");
  }

  void assignAt(const lox::parser::Token& name, std::any& value, int distance) {
    if (distance == 0) {
      assign(name, value);
    } else {
      ancestor(distance)->assign(name, value);
    }
  }

  std::any get(const lox::parser::Token& name) const {
    auto it = values_.find(name.lexeme);
    if (it != values_.end()) {
      return it->second;
    }
    if (parent_) {
      return parent_->get(name);
    }
    throw RuntimeError(name, "Undefined variable '" + name.lexeme + "'.");
  }

  std::any getAt(const lox::parser::Token& name, int distance) const {
    distance = std::max(distance, 0);
    if (distance == 0) {
      return get(name);
    } else {
      return ancestor(distance)->get(name);
    }
  }

  std::shared_ptr<Environment> ancestor(int distance) const {
    if (distance == 1) {
      return parent_;
    } else {
      return parent_->ancestor(distance - 1);
    }
  }

 private:
  std::unordered_map<std::string, std::any> values_;
  std::shared_ptr<Environment> parent_;
};

}  // namespace lang
}  // namespace lox