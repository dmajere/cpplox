#pragma once
#include <any>
#include <memory>

#include "ControlException.h"
#include "Expression.h"

namespace lox {
namespace parser {

struct StatementExpression;
struct Print;
struct Var;
struct Block;
struct If;
struct While;
struct Continue;
struct Break;
struct Return;
struct Function;
struct Class;

class StatementVisitor {
 public:
  virtual std::any visit(std::shared_ptr<const StatementExpression> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Print> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Var> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Block> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const If> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const While> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Continue> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Break> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Return> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Function> stmt) = 0;
  virtual std::any visit(std::shared_ptr<const Class> stmt) = 0;
  virtual ~StatementVisitor() = default;
};

struct Statement {
  virtual std::any accept(StatementVisitor* visitor) = 0;
  virtual ~Statement() = default;
};

struct StatementExpression
    : Statement,
      public std::enable_shared_from_this<StatementExpression> {
  StatementExpression(const std::shared_ptr<Expression>& expression)
      : expression{std::move(expression)} {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> expression;
};

struct Print : Statement, public std::enable_shared_from_this<Print> {
  Print(const std::shared_ptr<Expression>& expression)
      : expression{std::move(expression)} {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> expression;
};

struct Var : Statement, public std::enable_shared_from_this<Var> {
  Var(const Token& token, const std::shared_ptr<Expression>& initializer)
      : token(token), initializer(std::move(initializer)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const Token token;
  const std::shared_ptr<Expression> initializer;
};

struct Block : Statement, public std::enable_shared_from_this<Block> {
  Block(const std::vector<std::shared_ptr<Statement>>& statements)
      : statements(std::move(statements)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }
  const std::vector<std::shared_ptr<Statement>> statements;
};

struct If : public Statement, std::enable_shared_from_this<If> {
  If(const std::shared_ptr<Expression>& predicate,
     const std::shared_ptr<Statement>& then)
      : predicate(std::move(predicate)),
        then(std::move(then)),
        alternative(nullptr) {}

  If(const std::shared_ptr<Expression>& predicate,
     const std::shared_ptr<Statement>& then,
     const std::shared_ptr<Statement>& alternative)
      : predicate(std::move(predicate)),
        then(std::move(then)),
        alternative(std::move(alternative)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> predicate;
  const std::shared_ptr<Statement> then;
  const std::shared_ptr<Statement> alternative;
};

struct While : public Statement, std::enable_shared_from_this<While> {
  While(const std::shared_ptr<Expression>& condition,
        const std::shared_ptr<Statement>& body)
      : condition(std::move(condition)), body(std::move(body)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }
  const std::shared_ptr<Expression> condition;
  const std::shared_ptr<Statement> body;
};

struct Function : public Statement, std::enable_shared_from_this<Function> {
  Function(const Token& name, const std::vector<Token>& parameters,
           const std::shared_ptr<Block>& body)
      : name(name), parameters(std::move(parameters)), body(std::move(body)) {}
  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }
  const Token name;
  const std::vector<Token> parameters;
  const std::shared_ptr<Block> body;
};

struct Continue : public Statement, std::enable_shared_from_this<Continue> {
  explicit Continue(const Token& token) : token(token) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const Token token;
};

struct Break : public Statement, std::enable_shared_from_this<Break> {
  explicit Break(const Token& token) : token(token) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const Token token;
};

struct Return : public Statement, std::enable_shared_from_this<Return> {
  explicit Return(const Token& token) : token(token), value(nullptr) {}
  Return(const Token& token, const std::shared_ptr<Expression>& value)
      : token(token), value(std::move(value)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const Token token;
  const std::shared_ptr<Expression> value;
};

struct Class : public Statement, std::enable_shared_from_this<Class> {
  Class(const Token& name,
        const std::vector<std::shared_ptr<Function>>& methods)
      : name(name), methods(std::move(methods)) {}

  std::any accept(StatementVisitor* visitor) override {
    return visitor->visit(shared_from_this());
  }

  const Token name;
  const std::vector<std::shared_ptr<Function>> methods;
};

}  // namespace parser
}  // namespace lox