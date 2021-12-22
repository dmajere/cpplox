#pragma once
#include <any>
#include <memory>

#include "expression.h"

namespace lox {
namespace parser {

struct StatementExpression;
struct Print;
struct Var;

class StatementVisitor {
 public:
  virtual std::any visit(std::shared_ptr<const StatementExpression> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Print> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Var> stmt) = 0;
  virtual ~StatementVisitor() = default;
};

struct Statement {
  virtual std::any accept(StatementVisitor* visitor) = 0;
  virtual ~Statement() = default;
};

struct StatementExpression
    : Statement,
      public std::enable_shared_from_this<StatementExpression> {
  StatementExpression(std::shared_ptr<Expression> expression)
      : expression{std::move(expression)} {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> expression;
};

struct Print : Statement, public std::enable_shared_from_this<Print> {
  Print(std::shared_ptr<Expression> expression)
      : expression{std::move(expression)} {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> expression;
};

struct Var : Statement, public std::enable_shared_from_this<Var> {
  Var(const Token& token, std::shared_ptr<Expression> initializer)
      : token(token), initializer(std::move(initializer)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const Token token;
  const std::shared_ptr<Expression> initializer;
};

}  // namespace parser
}  // namespace lox