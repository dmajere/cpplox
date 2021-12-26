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
class LoxClass : public LoxCallable, std::enable_shared_from_this<LoxClass> {
 public:
  LoxClass(const std::string& name) : name_{name} {}
  LoxClass(const std::string& name,
           const std::unordered_map<std::string, std::shared_ptr<LoxFunction>>&
               methods)
      : name_{name}, methods_(std::move(methods)) {}

  std::string Name() const { return name_; }
  std::shared_ptr<LoxFunction> Method(const std::string& name) const {
    auto it = methods_.find(name);
    if (it != methods_.end()) {
      return it->second;
    }
    return nullptr;
  }

  std::any call(Interpreter* interpreter,
                const std::vector<std::any>& args) override {
    auto instance = std::make_shared<LoxInstance>(this);
    std::shared_ptr<LoxFunction> initializer = Method("init");

    return instance;
  }
  int arity() const override {
    auto init = Method("init");
    if (init) {
      return init->arity();
    }
    return 0;
  }

 private:
  const std::string name_;
  const std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods_;
};
}  // namespace lang
}  // namespace lox