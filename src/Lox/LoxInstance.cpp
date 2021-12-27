#include "LoxInstance.h"

#include "LoxClass.h"
#include "Token.h"
namespace lox {
namespace lang {

std::any LoxInstance::get(const lox::parser::Token& name) {
  auto it = fields_.find(name.lexeme);
  if (it != fields_.end()) {
    return it->second;
  }

  auto method = klass_->getMethod(name.lexeme);
  if (method) {
    return method->bind(shared_from_this());
  }

  throw RuntimeError(name, std::string(kUndefinedProperty));
}

void LoxInstance::set(const lox::parser::Token& name, std::any& value) {
  fields_.insert_or_assign(name.lexeme, value);
}

std::string LoxInstance::toString() const {
  return "Instance of " + klass_->toString();
}

}  // namespace lang
}  // namespace lox