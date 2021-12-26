#pragma once
#include <any>
#include <string>
#include <string_view>
#include <unordered_map>

#include "RuntimeError.h"

constexpr std::string_view kUndefinedProperty = "Undefined property";

namespace lox {
namespace lang {
class LoxClass;
class Token;

class LoxInstance {
 public:
  // LoxInstance(const std::shared_ptr<const LoxClass>& klass)
  //     : klass_(std::move(klass)) {}
  LoxInstance(const LoxClass* klass) : klass_(std::move(klass)) {}

  std::any get(const lox::parser::Token& name);
  void set(const lox::parser::Token& name, std::any& value);

 private:
  // std::shared_ptr<const LoxClass> klass_;
  const LoxClass* klass_;
  std::unordered_map<std::string, std::any> fields_;
};

}  // namespace lang
}  // namespace lox