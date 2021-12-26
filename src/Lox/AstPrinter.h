#pragma once
#include <any>
#include <string>

#include "Expression.h"
#include "Statement.h"

namespace lox {
namespace parser {

class AstPrinter : public ExpressionVisitor, StatementVisitor {
 public:
  std::string print(const std::shared_ptr<Expression>& expr);
  std::string print(const std::shared_ptr<Statement>& stmt);

  std::any visit(std::shared_ptr<const Binary> expr) override;
  std::any visit(std::shared_ptr<const Grouping> expr) override;
  std::any visit(std::shared_ptr<const Unary> expr) override;
  std::any visit(std::shared_ptr<const Literal> expr) override;
  std::any visit(std::shared_ptr<const Variable> expr) override;
  std::any visit(std::shared_ptr<const Sequence> expr) override;
  std::any visit(std::shared_ptr<const Ternary> expr) override;
  std::any visit(std::shared_ptr<const Assignment> expr) override;
  std::any visit(std::shared_ptr<const Call> expr) override;
  std::any visit(std::shared_ptr<const Lambda> expr) override;
  std::any visit(std::shared_ptr<const Get> expr) override;
  std::any visit(std::shared_ptr<const Set> expr) override;

  std::any visit(std::shared_ptr<const StatementExpression> stmt) override;
  std::any visit(std::shared_ptr<const Print> stmt) override;
  std::any visit(std::shared_ptr<const Var> stmt) override;
  std::any visit(std::shared_ptr<const Block> stmt) override;
  std::any visit(std::shared_ptr<const If> stmt) override;
  std::any visit(std::shared_ptr<const While> stmt) override;
  std::any visit(std::shared_ptr<const Continue> stmt) override;
  std::any visit(std::shared_ptr<const Break> stmt) override;
  std::any visit(std::shared_ptr<const Return> stmt) override;
  std::any visit(std::shared_ptr<const Function> stmt) override;
  std::any visit(std::shared_ptr<const Class> stmt) override;
};

}  // namespace parser
}  // namespace lox
