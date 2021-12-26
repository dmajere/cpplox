#pragma once
#include <any>
#include <string>
#include <string_view>
#include <unordered_map>

#include "LoxClass.h"
#include "RuntimeError.h"
#include "Token.h"

constexpr std::string_view kUndefinedProperty = "Undefined property";

namespace lox {
namespace lang {
class LoxClass;

class LoxInstance {
 public:
  LoxInstance(const LoxClass* klass) : klass_{klass} {}

  std::any get(const lox::parser::Token& name) {
    auto it = fields_.find(name.lexeme);
    if (it != fields_.end()) {
      return it->second;
    }
    throw RuntimeError(name, std::string(kUndefinedProperty));
  }

  void set(const lox::parser::Token& name, std::any& value) {
    fields_.insert_or_assign(name.lexeme, value);
  }

 private:
  const LoxClass* klass_;
  std::unordered_map<std::string, std::any> fields_;
};

}  // namespace lang
}  // namespace lox