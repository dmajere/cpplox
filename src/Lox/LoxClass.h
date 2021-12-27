#pragma once
#include <any>
#include <string>
#include <unordered_map>

#include "Interpreter.h"
#include "LoxCallable.h"
#include "LoxFunction.h"
#include "LoxInstance.h"

namespace lox {
namespace lang {
class LoxClass : public LoxCallable,
                 public std::enable_shared_from_this<LoxClass> {
 public:
  friend class LoxInstance;
  LoxClass(const std::string& name, const std::shared_ptr<LoxClass>& superclass,
           const std::unordered_map<std::string, std::shared_ptr<LoxFunction>>&
               methods)
      : name_{name}, superclass_(superclass), methods_(std::move(methods)) {}

  std::shared_ptr<LoxFunction> Method(const std::string& name) const {
    auto it = methods_.find(name);
    if (it != methods_.end()) {
      return it->second;
    }
    if (superclass_) {
      return superclass_->Method(name);
    }
    return nullptr;
  }

  std::any call(Interpreter& interpreter,
                const std::vector<std::any>& args) override {
    auto instance = std::make_shared<LoxInstance>(shared_from_this());
    std::shared_ptr<LoxFunction> initializer = Method("init");
    if (initializer != nullptr) {
      initializer->bind(instance)->call(interpreter, args);
    }
    return instance;
  }
  int arity() const override {
    auto init = Method("init");
    if (init) {
      return init->arity();
    }
    return 0;
  }

  std::string toString() const { return "Class " + name_; }

 private:
  const std::string name_;
  const std::shared_ptr<LoxClass> superclass_;
  const std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods_;
};
}  // namespace lang
}  // namespace lox