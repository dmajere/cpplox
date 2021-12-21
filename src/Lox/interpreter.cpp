#include "interpreter.h"

namespace lox {
namespace lang {

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Literal> literal) { return literal->value; }
std::any Interpreter::visit(std::shared_ptr<const lox::parser::Grouping> grouping) {
    return evaluate(grouping->expression);
}
std::any Interpreter::visit(std::shared_ptr<const lox::parser::Unary> unary) {
    auto right = evaluate(unary->right);

    switch (unary->op.type)
    {
    case lox::parser::Token::TokenType::MINUS:
        return -(std::any_cast<double>(right));
    case lox::parser::Token::TokenType::BANG:
        return isTruthy(right);
    case lox::parser::Token::TokenType::MINUS_MINUS:
        return std::any_cast<double>(right) - 1;
    case lox::parser::Token::TokenType::PLUS_PLUS:
        return std::any_cast<double>(right) + 1;
    default:
        return nullptr;
    }
}
std::any Interpreter::visit(std::shared_ptr<const lox::parser::Binary> binary) {
    auto left = evaluate(binary->left);
    auto right = evaluate(binary->right);
    auto& left_type = left.type();
    auto& right_type = right.type();
    //TODO: type comprehension

    switch (binary->op.type)
    {
    case lox::parser::Token::TokenType::BANG_EQUAL:
        return !isEqual(left, right);
    case lox::parser::Token::TokenType::EQUAL_EQUAL:
        return isEqual(left, right);
    case lox::parser::Token::TokenType::GREATER:
        return std::any_cast<double&>(left) > std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::GREATER_EQUAL:
        return std::any_cast<double&>(left) >= std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::LESS:
        return std::any_cast<double&>(left) < std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::LESS_EQUAL:
        return std::any_cast<double&>(left) <= std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::PLUS:
        if (left_type == typeid(double) && right_type == typeid(double)) {
            return std::any_cast<double&>(left) + std::any_cast<double&>(right);
        }
        if (left_type == typeid(std::string) && right_type == typeid(std::string)) {
            return std::any_cast<std::string>(left) + std::any_cast<std::string>(right);
        }
    case lox::parser::Token::TokenType::MINUS:
        return std::any_cast<double>(left) - std::any_cast<double>(right);
    case lox::parser::Token::TokenType::STAR:
        return std::any_cast<double>(left) * std::any_cast<double>(right);
    case lox::parser::Token::TokenType::SLASH:
        return std::any_cast<double>(left) / std::any_cast<double>(right);
    default:
        break;
    }

    return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Block> block) {
    for (int i = 0; i < block->expressions.size(); i++) {
        if (i == block->expressions.size() - 1) {
            return evaluate(block->expressions[i]);
        }
        evaluate(block->expressions[i]);
    }
    return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Condition> condition) {
    auto predicate = evaluate(condition->predicate);
    if (isTruthy(predicate)) {
        return evaluate(condition->then);
    }
    if (condition->alternative) {
        return evaluate(condition->alternative);
    }
    return nullptr;
}


std::any Interpreter::evaluate(std::shared_ptr<lox::parser::Expression> expr) {
    return expr->accept(this);
}
bool Interpreter::isTruthy(const std::any& object) const {
if (!object.has_value()) {
    return false;
}

auto& object_type = object.type();
if (object_type == typeid(nullptr)) {
    return false;
} 
if (object_type == typeid(bool)) {
    return std::any_cast<bool>(object);
}
return true;
}

bool Interpreter::isEqual(const std::any& left,const std::any& right) const {
    if (!left.has_value() && !right.has_value()) {
        return true;
    }
    if (!left.has_value()) {
        return false;
    }

    auto& left_type = left.type();
    auto& right_type = right.type();

    if (left_type == right_type) {
        if (left_type == typeid(std::string)) {
           return std::any_cast<std::string>(left) ==  std::any_cast<std::string>(right);
        }
        if (left_type == typeid(double)) {
           return std::any_cast<double>(left) ==  std::any_cast<double>(right);
        }
        return false;
    }
    return false;
}


}  // namespace lang
}  // namespace lox