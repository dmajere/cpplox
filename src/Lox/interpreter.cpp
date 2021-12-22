#include "interpreter.h"

#include "environment.h"
#include "lox.h"

namespace lox {
namespace lang {

std::any Interpreter::visit(
    std::shared_ptr<const lox::parser::Literal> expr) {
  return expr->value;
}
std::any Interpreter::visit(
    std::shared_ptr<const lox::parser::Grouping> expr) {
  return evaluate(expr->expression);
}
std::any Interpreter::visit(std::shared_ptr<const lox::parser::Unary> expr) {
  auto right = evaluate(expr->right);

  switch (expr->op.type) {
    case lox::parser::Token::TokenType::MINUS:
      checkNumberOperand(expr->op, right);
      return -(std::any_cast<double>(right));
    case lox::parser::Token::TokenType::BANG:
      return isTruthy(right);
    case lox::parser::Token::TokenType::MINUS_MINUS:
      checkNumberOperand(expr->op, right);
      return std::any_cast<double>(right) - 1;
    case lox::parser::Token::TokenType::PLUS_PLUS:
      checkNumberOperand(expr->op, right);
      return std::any_cast<double>(right) + 1;
    default:
      return nullptr;
  }
}
std::any Interpreter::visit(std::shared_ptr<const lox::parser::Binary> expr) {
  auto left = evaluate(expr->left);
  auto right = evaluate(expr->right);
  auto& left_type = left.type();
  auto& right_type = right.type();
  // TODO: type comprehension

  switch (expr->op.type) {
    case lox::parser::Token::TokenType::BANG_EQUAL:
      return !isEqual(left, right);
    case lox::parser::Token::TokenType::EQUAL_EQUAL:
      return isEqual(left, right);
    case lox::parser::Token::TokenType::GREATER:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double&>(left) > std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::GREATER_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double&>(left) >= std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::LESS:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double&>(left) < std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::LESS_EQUAL:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double&>(left) <= std::any_cast<double&>(right);
    case lox::parser::Token::TokenType::PLUS:
      if (left_type == typeid(double) && right_type == typeid(double)) {
        return std::any_cast<double&>(left) + std::any_cast<double&>(right);
      }
      if (left_type == typeid(std::string) &&
          right_type == typeid(std::string)) {
        return std::any_cast<std::string>(left) +
               std::any_cast<std::string>(right);
      }
      if (left_type == typeid(std::string) ||
          right_type == typeid(std::string)) {
        return toString(left) + toString(right);
      }
      throw RuntimeError(expr->op,
                         "Operands must be either numbers or strings.");
    case lox::parser::Token::TokenType::MINUS:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) - std::any_cast<double>(right);
    case lox::parser::Token::TokenType::STAR:
      checkNumberOperands(expr->op, left, right);
      return std::any_cast<double>(left) * std::any_cast<double>(right);
    case lox::parser::Token::TokenType::SLASH:
      checkNumberOperands(expr->op, left, right);
      if (std::any_cast<double>(right) == 0) {
        throw ZeroDivision(expr->op, "Second operand must be non-zero.");
      }
      return std::any_cast<double>(left) / std::any_cast<double>(right);
    default:
      break;
  }

  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Block> expr) {
  for (int i = 0; i < expr->expressions.size(); i++) {
    if (i == expr->expressions.size() - 1) {
      return evaluate(expr->expressions[i]);
    }
    evaluate(expr->expressions[i]);
  }
  return nullptr;
}

std::any Interpreter::visit(
    std::shared_ptr<const lox::parser::Condition> expr) {
  auto predicate = evaluate(expr->predicate);
  if (isTruthy(predicate)) {
    return evaluate(expr->then);
  }
  if (expr->alternative) {
    return evaluate(expr->alternative);
  }
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Variable> expr) {
  return env_->get(expr->token);
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Assignment> expr) {
    auto value = evaluate(expr->target);
    env_->assign(expr->token, value);
    return nullptr;
}

std::any Interpreter::visit(
    std::shared_ptr<const lox::parser::StatementExpression> stmt) {
  evaluate(stmt->expression);
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Print> stmt) {
  std::cout << toString(evaluate(stmt->expression));
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Var> stmt) {
  std::any value = stmt->initializer ? evaluate(stmt->initializer) : nullptr;
  env_->define(stmt->token.lexeme, value);
  return nullptr;
}


std::any Interpreter::evaluate(std::shared_ptr<lox::parser::Expression> expr) {
  return expr->accept(this);
}
std::any Interpreter::evaluate(
    std::vector<std::shared_ptr<lox::parser::Statement>> stmt) {
  for (auto& s : stmt) {
    try {
      execute(s);
    } catch (RuntimeError& error) {
      lox::lang::Lox::runtime_error(error);
    }
  }
  return nullptr;
}

void Interpreter::execute(std::shared_ptr<lox::parser::Statement>& stmt) {
  stmt->accept(this);
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

bool Interpreter::isEqual(const std::any& left, const std::any& right) const {
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
      return std::any_cast<std::string>(left) ==
             std::any_cast<std::string>(right);
    }
    if (left_type == typeid(double)) {
      return std::any_cast<double>(left) == std::any_cast<double>(right);
    }
    return false;
  }
  return false;
}
bool Interpreter::checkType(const std::any& object,
                            const std::type_info& type) const {
  return object.type() == type;
}

void Interpreter::checkNumberOperand(const lox::parser::Token& token,
                                     const std::any& object) const {
  if (checkType(object, typeid(double))) return;
  throw RuntimeError(token, "Operand must be number.");
}

void Interpreter::checkNumberOperands(const lox::parser::Token& token,
                                      const std::any& left,
                                      const std::any& right) const {
  auto& dti = typeid(double);
  if (checkType(left, dti) && checkType(right, dti)) return;
  throw RuntimeError(token, "Operands must be numbers.");
}

}  // namespace lang
}  // namespace lox