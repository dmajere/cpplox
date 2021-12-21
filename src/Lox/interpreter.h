#pragma once
#include <any>

#include "expression.h"

namespace lox {
namespace lang {

class Interpreter : public lox::parser::AstVisitor {
 public:
  struct RuntimeError : public std::runtime_error {
    using std::runtime_error::runtime_error;
    RuntimeError(const lox::parser::Token token, const std::string& message) : std::runtime_error(message), token(token) {}
    const lox::parser::Token token;
  };
  struct ZeroDivision : public RuntimeError {
    ZeroDivision(const lox::parser::Token token, const std::string& message) : RuntimeError(token, message) {}
  };

  std::any visit(std::shared_ptr<const lox::parser::Literal> literal);
  std::any visit(std::shared_ptr<const lox::parser::Grouping> grouping);
  std::any visit(std::shared_ptr<const lox::parser::Unary> unary);
  std::any visit(std::shared_ptr<const lox::parser::Binary> binary);
  std::any visit(std::shared_ptr<const lox::parser::Block> block);
  std::any visit(std::shared_ptr<const lox::parser::Condition> condition);
  std::any evaluate(std::shared_ptr<lox::parser::Expression> expr);

  static std::string toString(const std::any& object) {
    auto& object_type = object.type();

    if (!object.has_value()) {
      return "";
    }
    if (object_type == typeid(nullptr)) {
      return "nil";
    }
    if (object_type == typeid(std::string)) {
      return std::any_cast<std::string>(object);
    }
    if (object_type == typeid(bool)) {
      return std::any_cast<bool>(object) ? "true" : "false";
    }
    if (object_type == typeid(double)) {
      return std::to_string(std::any_cast<double>(object));
    }
    return "";
}

private:
  bool isTruthy(const std::any& object) const;
  bool isEqual(const std::any& left,const std::any& right) const;
  bool checkType(const std::any& object, const std::type_info& type) const;
  void checkNumberOperand(const lox::parser::Token& token, const std::any& object) const;
  void checkNumberOperands(const lox::parser::Token& token, const std::any& left, const std::any& right) const;

};

}  // namespace lang
}  // namespace lox