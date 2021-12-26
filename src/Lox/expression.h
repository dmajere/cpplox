#pragma once

#include <any>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "Token.h"

namespace lox {
namespace parser {

struct Expression;
struct Binary;
struct Grouping;
struct Unary;
struct Literal;
struct Variable;
struct Sequence;
struct Ternary;
struct Assignment;
struct Call;
struct Lambda;
struct Function;
struct Get;

class ExpressionVisitor {
 public:
  virtual std::any visit(std::shared_ptr<const Binary> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Grouping> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Unary> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Literal> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Variable> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Sequence> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Ternary> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Assignment> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Call> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Lambda> expr) = 0;
  virtual std::any visit(std::shared_ptr<const Get> expr) = 0;
  virtual ~ExpressionVisitor() = default;
};

struct Expression {
  virtual std::any accept(ExpressionVisitor* visitor) const = 0;
  virtual ~Expression() = default;
};

struct Binary : Expression, public std::enable_shared_from_this<Binary> {
  Binary(const std::shared_ptr<Expression>& left, const Token& op,
         const std::shared_ptr<Expression>& right)
      : left(std::move(left)), op(op), right(std::move(right)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> left;
  const Token op;
  const std::shared_ptr<Expression> right;
};

struct Grouping : public Expression, std::enable_shared_from_this<Grouping> {
  Grouping(const std::shared_ptr<Expression>& exp)
      : expression(std::move(exp)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> expression;
};

struct Unary : public Expression, std::enable_shared_from_this<Unary> {
  Unary(const Token& op, const std::shared_ptr<Expression>& right)
      : op(op), right(std::move(right)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const Token op;
  const std::shared_ptr<Expression> right;
};

struct Literal : public Expression, std::enable_shared_from_this<Literal> {
  Literal(const std::any& value) : value(value) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::any value;
};

struct Variable : public Expression, std::enable_shared_from_this<Variable> {
  Variable(const Token& token) : token(token) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }
  const Token token;
};

struct Sequence : public Expression, std::enable_shared_from_this<Sequence> {
  Sequence() {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  std::vector<const std::shared_ptr<Expression>> expressions;
};

struct Ternary : public Expression, std::enable_shared_from_this<Ternary> {
  Ternary(const std::shared_ptr<Expression>& predicate,
          const std::shared_ptr<Expression>& then)
      : predicate(std::move(predicate)),
        then(std::move(then)),
        alternative(nullptr) {}

  Ternary(const std::shared_ptr<Expression>& predicate,
          const std::shared_ptr<Expression>& then,
          const std::shared_ptr<Expression>& alternative)
      : predicate(std::move(predicate)),
        then(std::move(then)),
        alternative(std::move(alternative)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> predicate;
  const std::shared_ptr<Expression> then;
  const std::shared_ptr<Expression> alternative;
};

struct Assignment : Expression,
                    public std::enable_shared_from_this<Assignment> {
  Assignment(const Token& token, const std::shared_ptr<Expression>& target)
      : token(token), target(std::move(target)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const Token token;
  const std::shared_ptr<Expression> target;
};

struct Call : Expression, public std::enable_shared_from_this<Call> {
  Call(const std::shared_ptr<Expression>& callee, const Token& paren,
       const std::shared_ptr<Expression>& arguments)
      : callee(std::move(callee)),
        paren(paren),
        arguments(std::move(arguments)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> callee;
  const Token paren;
  const std::shared_ptr<Expression> arguments;
};

struct Lambda : public Expression, std::enable_shared_from_this<Lambda> {
  explicit Lambda(const std::shared_ptr<Function>& function)
      : function(std::move(function)) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Function> function;
};

struct Get : public Expression, std::enable_shared_from_this<Get> {
  Get(const std::shared_ptr<Expression>& object, const Token& name)
      : object(std::move(object)), name(name) {}

  std::any accept(ExpressionVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> object;
  const Token name;
};

}  // namespace parser
}  // namespace lox