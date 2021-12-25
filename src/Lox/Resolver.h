#pragma once
#include <any>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "Expression.h"
#include "Interpreter.h"
#include "Statement.h"

namespace lox {
namespace lang {

class Resolver : public lox::parser::ExpressionVisitor,
                 lox::parser::StatementVisitor {
 public:
  Resolver(std::shared_ptr<Interpreter> interpreter);
  ~Resolver();
  void resolve(
      const std::vector<std::shared_ptr<lox::parser::Statement>>& statements);

  std::any visit(std::shared_ptr<const lox::parser::Binary> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Grouping> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Unary> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Literal> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Variable> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Sequence> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Ternary> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Assignment> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Call> expr) override;
  std::any visit(std::shared_ptr<const lox::parser::Lambda> expr) override;

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

 private:
  enum class FunctionType { None, Function };

  const std::shared_ptr<Interpreter> interpreter_;
  FunctionType currentFunction_;
  std::vector<std::unordered_map<std::string, bool>> scopes_;

  void resolve(const std::shared_ptr<lox::parser::Statement>& stmt);
  void resolve(const std::shared_ptr<lox::parser::Expression>& expr);
  void resolve(std::shared_ptr<const lox::parser::Expression> expr,
               const lox::parser::Token& name);
  void resolve(std::shared_ptr<const lox::parser::Function> func,
               FunctionType type);
  void beginScope();
  void endScope();
  void declare(const lox::parser::Token& name);
  void define(const lox::parser::Token& name);
};

}  // namespace lang
}  // namespace lox
