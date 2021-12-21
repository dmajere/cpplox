#pragma once

#include <any>
#include <sstream>
#include <string>
#include <vector>
#include <memory>

#include "token.h"

namespace lox {
namespace parser {

struct Expression;
struct Binary;
struct Grouping;
struct Unary;
struct Literal;
struct Block;
struct Condition;

class AstVisitor {
 public:
  virtual std::any visit(std::shared_ptr<const Binary> binary) = 0;
  virtual std::any visit(std::shared_ptr<const Grouping> grouping) = 0;
  virtual std::any visit(std::shared_ptr<const Unary> unary) = 0;
  virtual std::any visit(std::shared_ptr<const Literal> literal) = 0;
  virtual std::any visit(std::shared_ptr<const Block> block) = 0;
  virtual std::any visit(std::shared_ptr<const Condition> condition) = 0;
  virtual ~AstVisitor() = default;
};

struct Expression {
  virtual std::any accept(AstVisitor* visitor) const = 0;
  virtual ~Expression(){};
};

struct Binary : Expression, public std::enable_shared_from_this<Binary> {
  Binary(const std::shared_ptr<Expression>& left, const Token& op,
         const std::shared_ptr<Expression>& right)
      : left(std::move(left)), op(op), right(std::move(right)) {}
  ~Binary() {}

  std::any accept(AstVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> left;
  const Token op;
  const std::shared_ptr<Expression> right;
};

struct Grouping : public Expression, std::enable_shared_from_this<Grouping> {
  Grouping(const std::shared_ptr<Expression>& exp) : expression(std::move(exp)) {}
  ~Grouping() {}
  std::any accept(AstVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> expression;
};

struct Unary : public Expression, std::enable_shared_from_this<Unary> {
  Unary(const Token& op, const std::shared_ptr<Expression>& right)
      : op(op), right(std::move(right)) {}
  ~Unary() {}
  std::any accept(AstVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const Token op;
  const std::shared_ptr<Expression> right;
};

struct Literal : public Expression, std::enable_shared_from_this<Literal> {
  Literal(const std::any value) : value(value) {}
  ~Literal() {}
  std::any accept(AstVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::any value;
};

struct Block : public Expression, std::enable_shared_from_this<Block> {
  Block() : expressions{} {}
  ~Block() {}

  std::any accept(AstVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }
  std::vector<const std::shared_ptr<Expression>> expressions;
};

struct Condition : public Expression, std::enable_shared_from_this<Condition> {
  Condition(const std::shared_ptr<Expression>& predicate,
            const std::shared_ptr<Expression>& then)
      : predicate(std::move(predicate)),
        then(std::move(then)),
        alternative(nullptr) {}

  Condition(const std::shared_ptr<Expression>& predicate,
            const std::shared_ptr<Expression>& then,
            const std::shared_ptr<Expression>& alternative)
      : predicate(std::move(predicate)),
        then(std::move(then)),
        alternative(std::move(alternative)) {}
  ~Condition() {}

  std::any accept(AstVisitor* visitor) const override {
    return visitor->visit(shared_from_this());
  }

  const std::shared_ptr<Expression> predicate;
  const std::shared_ptr<Expression> then;
  const std::shared_ptr<Expression> alternative;
};

}  // namespace parser
}  // namespace lox