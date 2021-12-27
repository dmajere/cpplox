#include "LoxClass.h"

namespace lox {
namespace lang {

int LoxClass::arity() const {
  auto init = getMethod("init");
  if (init) {
    return init->arity();
  }
  return 0;
}

std::any LoxClass::call(Interpreter& interpreter,
                        const std::vector<std::any>& args) {
  auto instance = std::make_shared<LoxInstance>(shared_from_this());
  std::shared_ptr<LoxFunction> initializer = getMethod("init");
  if (initializer != nullptr) {
    initializer->bind(instance)->call(interpreter, args);
  }
  return instance;
}

std::shared_ptr<LoxFunction> LoxClass::getMethod(
    const std::string& name) const {
  auto it = methods_.find(name);
  if (it != methods_.end()) {
    return it->second;
  }
  if (superclass_) {
    return superclass_->getMethod(name);
  }
  return nullptr;
}

std::string LoxClass::toString() const { return "Class " + name_; }

}  // namespace lang
}  // namespace lox