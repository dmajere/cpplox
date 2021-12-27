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

  int arity() const override;
  std::any call(Interpreter& interpreter,
                const std::vector<std::any>& args) override;

  std::shared_ptr<LoxFunction> getMethod(const std::string& name) const;
  std::string toString() const;

 private:
  const std::string name_;
  const std::shared_ptr<LoxClass> superclass_;
  const std::unordered_map<std::string, std::shared_ptr<LoxFunction>> methods_;
};
}  // namespace lang
}  // namespace lox