#pragma once
#include <any>

#include "expression.h"

namespace lox {
namespace lang {

class Interpreter : public lox::parser::AstVisitor {
 public:
  std::any visit(std::shared_ptr<const lox::parser::Literal> literal);
  std::any visit(std::shared_ptr<const lox::parser::Grouping> grouping);
  std::any visit(std::shared_ptr<const lox::parser::Unary> unary);
  std::any visit(std::shared_ptr<const lox::parser::Binary> binary);
  std::any visit(std::shared_ptr<const lox::parser::Block> block);
  std::any visit(std::shared_ptr<const lox::parser::Condition> condition);
  std::any evaluate(std::shared_ptr<lox::parser::Expression> expr);

private:
  bool isTruthy(const std::any& object) const;
  bool isEqual(const std::any& left,const std::any& right) const;

};

}  // namespace lang
}  // namespace lox