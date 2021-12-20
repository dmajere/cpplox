#pragma once
#include <any>
#include <string>
#include <sstream>
#include <type_traits>
#include <Folly/Conv.h>
#include "token.h"

namespace lox {
namespace parser {

class AstVisitor;
class Expression;
class Binary;
class Grouping;
class Unary;

class Literal;

class AstVisitor {
    public:
    virtual void visit(const Binary* binary) = 0;
    virtual void visit(const Grouping* grouping) = 0;
    virtual void visit(const Unary* unary) = 0;
    virtual void visit(const Literal* literal) = 0;
};

class Expression {
    public:
    virtual ~Expression() {}
    virtual const std::string toString() const = 0;
    virtual void accept(AstVisitor* visitor) const = 0;
};


class Binary : public Expression {
    public:
    Binary(std::unique_ptr<Expression> left, Token& op, std::unique_ptr<Expression> right) 
        : left_(std::move(left)), op_(op), right_(std::move(right)) {}
    // ~Binary() {}
    void accept(AstVisitor* visitor) const override {
        visitor->visit(this);
    }
    const std::string toString() const override {
        std::stringstream ss;
        ss << left_->toString() 
           << " "  << op_ << " " 
           << right_->toString();
        return ss.str();
    }

    const Expression* getLeft() const {
        return left_.get();
    }
    const Token& getToken() const {
        return op_;
    }
    const Expression* getRight() const {
        return right_.get();
    }


    private:
    std::unique_ptr<Expression> left_;
    const Token op_;
    std::unique_ptr<Expression> right_;
};

class Grouping : public Expression {
    public:
    Grouping(std::unique_ptr<Expression> exp) : expression_(std::move(exp)) {}
    // ~Grouping() {}
    void accept(AstVisitor* visitor) const override {
        visitor->visit(this);
    }
    const std::string toString() const override {
        std::stringstream ss;
        ss << expression_->toString();
        return ss.str();
    }

    private:
    std::unique_ptr<Expression> expression_;
};

class Literal : public Expression {
    public:
    Literal(const std::any value) : value_(value) {}
    // ~Literal() {}
    void accept(AstVisitor* visitor) const override {
        visitor->visit(this);
    }
    const std::string toString() const override {
        auto& value_type = value_.type();
        if (value_type == typeid(nullptr)) {
            return "nil";
        } else if (value_type == typeid(std::string)) {
            return std::any_cast<std::string>(value_);
        } else if (value_type == typeid(double)) {
            return std::to_string(std::any_cast<double>(value_));
        } else if (value_type == typeid(bool)) {
            return std::any_cast<bool>(value_) ? "true" : "false";
        }
        return "";
    }

    private:
    const std::any value_;
};

class Unary : public Expression {
    public:
    Unary(Token& op, std::unique_ptr<Expression> right) : op_(op), right_(std::move(right)) {}
    // ~Unary() {}
    void accept(AstVisitor* visitor) const override {
        visitor->visit(this);
    }
    const std::string toString() const override {
        std::stringstream ss;
        ss << op_ << " " << right_->toString();
        return ss.str();
    }
    
    private:
    const Token op_;
    std::unique_ptr<Expression> right_;
};


}
}