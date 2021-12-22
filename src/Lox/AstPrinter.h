#pragma once
#include <any>
#include <string>

#include "expression.h"
#include "statement.h"

namespace lox {
namespace parser {

class AstPrinter : public AstVisitor, StatementVisitor {
 public:
  std::string print(std::shared_ptr<Expression> expr);
  std::string print(const std::shared_ptr<Statement> stmt);
  std::any visit(std::shared_ptr<const Binary> expr) override;
  std::any visit(std::shared_ptr<const Grouping> expr) override;
  std::any visit(std::shared_ptr<const Unary> expr) override;
  std::any visit(std::shared_ptr<const Literal> expr) override;
  std::any visit(std::shared_ptr<const Variable> expr) override;
  std::any visit(std::shared_ptr<const Sequence> expr) override;
  std::any visit(std::shared_ptr<const Condition> expr) override;
  std::any visit(std::shared_ptr<const Assignment> expr) override;

  std::any visit(std::shared_ptr<const StatementExpression> stmt) override;
  std::any visit(std::shared_ptr<const Print> stmt) override;
  std::any visit(std::shared_ptr<const Var> stmt) override;
  std::any visit(std::shared_ptr<const Block> stmt) override;
};

}  // namespace parser
}  // namespace lox
