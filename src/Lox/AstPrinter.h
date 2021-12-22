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
  std::any visit(std::shared_ptr<const Binary> binary) override;
  std::any visit(std::shared_ptr<const Grouping> grouping) override;
  std::any visit(std::shared_ptr<const Unary> unary) override;
  std::any visit(std::shared_ptr<const Literal> literal) override;
  std::any visit(std::shared_ptr<const Block> block) override;
  std::any visit(std::shared_ptr<const Condition> condition) override;
  std::any visit(std::shared_ptr<const StatementExpression> stmt) override;
  std::any visit(std::shared_ptr<const Print> stmt) override;
  std::any visit(std::shared_ptr<const Variable> var) override;
  std::any visit(std::shared_ptr<const Var> stmt) override;
};

}  // namespace parser
}  // namespace lox
