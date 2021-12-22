#pragma once
#include <any>
#include <vector>

#include "RuntimeError.h"
#include "environment.h"
#include "expression.h"
#include "statement.h"

namespace lox {
namespace lang {

class Interpreter : public lox::parser::AstVisitor,
                    lox::parser::StatementVisitor {
 public:
  Interpreter() : env_(std::make_shared<Environment>()) {}

  std::any evaluate(std::shared_ptr<lox::parser::Expression> expr);
  std::any evaluate(std::vector<std::shared_ptr<lox::parser::Statement>> expr);

  std::any visit(std::shared_ptr<const lox::parser::Literal> literal) override;
  std::any visit(std::shared_ptr<const lox::parser::Variable> var) override;
  std::any visit(
      std::shared_ptr<const lox::parser::Grouping> grouping) override;
  std::any visit(std::shared_ptr<const lox::parser::Unary> unary) override;
  std::any visit(std::shared_ptr<const lox::parser::Binary> binary) override;
  std::any visit(std::shared_ptr<const lox::parser::Block> block) override;
  std::any visit(
      std::shared_ptr<const lox::parser::Condition> condition) override;
  std::any visit(
      std::shared_ptr<const lox::parser::StatementExpression> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Print> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Var> stmt) override;

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
  std::shared_ptr<Environment> env_;

  bool isTruthy(const std::any& object) const;
  bool isEqual(const std::any& left, const std::any& right) const;
  bool checkType(const std::any& object, const std::type_info& type) const;
  void checkNumberOperand(const lox::parser::Token& token,
                          const std::any& object) const;
  void checkNumberOperands(const lox::parser::Token& token,
                           const std::any& left, const std::any& right) const;
  void execute(std::shared_ptr<lox::parser::Statement>& stmt);
};

}  // namespace lang
}  // namespace lox