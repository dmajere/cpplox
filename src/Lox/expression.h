#pragma once
#include <any>
#include <string>
#include <sstream>
#include <vector>
#include <Folly/Conv.h>
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
    virtual std::any visit(const Binary* binary) = 0;
    virtual std::any visit(const Grouping* grouping) = 0;
    virtual std::any visit(const Unary* unary) = 0;
    virtual std::any visit(const Literal* literal) = 0;
    virtual std::any visit(const Block* block) = 0;
    virtual std::any visit(const Condition* condition) = 0;
    virtual ~AstVisitor() = default;
};

struct Expression {
    virtual std::any accept(AstVisitor* visitor) const = 0;
    virtual ~Expression() {};
};


struct Binary : public Expression {
    Binary(
        std::unique_ptr<Expression> left, 
        const Token& op, 
        std::unique_ptr<Expression> right) 
    : left(std::move(left))
    , op(op)
    , right(std::move(right)) {}
    ~Binary() {}

    std::any accept(AstVisitor* visitor) const override {
        return visitor->visit(this);
    }

    std::unique_ptr<Expression> left;
    const Token op;
    std::unique_ptr<Expression> right;
};

struct Grouping : public Expression {
    Grouping(std::unique_ptr<Expression> exp) 
    : expression(std::move(exp)) {}
    ~Grouping() {}
    std::any accept(AstVisitor* visitor) const override {
        return visitor->visit(this);
    }

    std::unique_ptr<Expression> expression;
};

struct Unary : public Expression {
    Unary(
        const Token& op, 
        std::unique_ptr<Expression> right) 
    : op(op)
    , right(std::move(right)) {}
    ~Unary() {}
    std::any accept(AstVisitor* visitor) const override {
        return visitor->visit(this);
    }

    const Token op;
    std::unique_ptr<Expression> right;
};


struct Literal : public Expression {
    Literal(const std::any value) : value(value) {}
    ~Literal() {}
    std::any accept(AstVisitor* visitor) const override {
        return visitor->visit(this);
    }

    const std::any value;
};

struct Block : public Expression {
    Block() : expressions{} {}
    ~Block() {}

    std::any accept(AstVisitor* visitor) const override {
        return visitor->visit(this);
    }
    std::vector<std::unique_ptr<Expression>> expressions;
};

struct Condition : public Expression {
    Condition(
        std::unique_ptr<Expression> predicate,
        std::unique_ptr<Expression> then)
    : predicate(std::move(predicate))
    , then(std::move(then))
    , alternative(nullptr) {}

    Condition(
        std::unique_ptr<Expression> predicate,
        std::unique_ptr<Expression> then,
        std::unique_ptr<Expression> alternative) 
    : predicate(std::move(predicate))
    , then(std::move(then))
    , alternative(std::move(alternative)) {}
    ~Condition() {}

    std::any accept(AstVisitor* visitor) const override {
        return visitor->visit(this);
    }

    std::unique_ptr<Expression> predicate;
    std::unique_ptr<Expression> then;
    std::unique_ptr<Expression> alternative;
};

}
}