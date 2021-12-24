#pragma once
#include <any>
#include <unordered_map>
#include <vector>

#include "Environment.h"
#include "Expression.h"
#include "RuntimeError.h"
#include "Statement.h"

namespace lox {
namespace lang {

class Interpreter : public lox::parser::ExpressionVisitor,
                    lox::parser::StatementVisitor {
 public:
  Interpreter();

  void evaluate(
      const std::vector<std::shared_ptr<lox::parser::Statement>>& stmt);
  void evaluate(const std::shared_ptr<lox::parser::Block>& stmt,
                std::shared_ptr<Environment> env);
  void resolve(std::shared_ptr<const lox::parser::Expression> expr, int depth);

  // AstVisitor
  std::any visit(std::shared_ptr<const lox::parser::Literal> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Variable> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Grouping> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Unary> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Binary> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Sequence> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Ternary> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Assignment> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Call> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Lambda> expr) override;

  // StatementVisitor
  std::any visit(
      std::shared_ptr<const lox::parser::StatementExpression> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Print> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Var> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Block> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::If> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::While> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Continue> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Break> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Return> stmt) override;
  std::any visit(std::shared_ptr<const lox::parser::Function> stmt) override;

  std::shared_ptr<Environment> environment() const { return env_; }

 private:
  std::shared_ptr<Environment> globals_;
  std::shared_ptr<Environment> env_;
  std::unordered_map<std::shared_ptr<const lox::parser::Expression>, int>
      locals_;

  std::any evaluate(const std::shared_ptr<lox::parser::Expression>& expr);
  void execute(const std::shared_ptr<lox::parser::Statement>& stmt);
  void execute(
      const std::vector<std::shared_ptr<lox::parser::Statement>>& statements,
      std::shared_ptr<Environment> env);

  bool isTruthy(const std::any& object) const;
  bool isEqual(const std::any& left, const std::any& right) const;
  bool checkType(const std::any& object, const std::type_info& type) const;
  void checkNumberOperand(const lox::parser::Token& token,
                          const std::any& object) const;
  void checkNumberOperands(const lox::parser::Token& token,
                           const std::any& left, const std::any& right) const;
  std::any lookupVariable(const lox::parser::Token& name,
                          std::shared_ptr<const lox::parser::Expression> expr);
};

}  // namespace lang
}  // namespace lox