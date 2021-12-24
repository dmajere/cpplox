#include "Interpreter.h"

#include "ControlException.h"
#include "LoxCallable.h"
#include "LoxFunction.h"
#include "LoxNative.h"
#include "lox.h"
#include "utils.h"

namespace lox {
namespace lang {

Interpreter::Interpreter() : globals_(std::make_shared<Environment>()) {
  auto clock_ptr = std::make_shared<Clock>();
  auto clock = std::make_any<std::shared_ptr<LoxCallable>>(clock_ptr);
  globals_->define("clock", clock);
  env_ = globals_;
}

void Interpreter::evaluate(
    const std::vector<std::shared_ptr<lox::parser::Statement>>& stmt) {
  for (auto& s : stmt) {
    try {
      if (s) {
        execute(s);
      }
    } catch (RuntimeError& error) {
      lox::lang::Lox::runtime_error(error);
    }
  }
}

void Interpreter::evaluate(const std::shared_ptr<lox::parser::Block>& block,
                           std::shared_ptr<Environment> env) {
  execute(block->statements, env);
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Literal> expr) {
  return expr->value;
}
std::any Interpreter::visit(std::shared_ptr<const lox::parser::Grouping> expr) {
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
  switch (expr->op.type) {
    case lox::parser::Token::TokenType::AND:
      if (isTruthy(left)) {
        return isTruthy(evaluate(expr->right));
      }
      return false;
    case lox::parser::Token::TokenType::OR:
      if (isTruthy(left)) {
        return true;
      }
      return isTruthy(evaluate(expr->right));
    default:
      break;
  }

  auto right = evaluate(expr->right);
  auto& left_type = left.type();
  auto& right_type = right.type();

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
        return lox::util::any_to_string(left) + lox::util::any_to_string(right);
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

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Sequence> expr) {
  for (int i = 0; i < expr->expressions.size(); i++) {
    if (i == expr->expressions.size() - 1) {
      return evaluate(expr->expressions[i]);
    }
    evaluate(expr->expressions[i]);
  }
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Ternary> expr) {
  auto predicate = evaluate(expr->predicate);
  if (isTruthy(predicate)) {
    return evaluate(expr->then);
  } else if (expr->alternative) {
    return evaluate(expr->alternative);
  }
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Variable> expr) {
  return env_->get(expr->token);
}

std::any Interpreter::visit(
    std::shared_ptr<const lox::parser::Assignment> expr) {
  auto value = evaluate(expr->target);
  env_->assign(expr->token, value);
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Call> expr) {
  auto callee = evaluate(expr->callee);
  std::vector<std::any> args;
  if (lox::parser::Sequence* seq =
          dynamic_cast<lox::parser::Sequence*>(expr->arguments.get())) {
    for (const auto& arg : seq->expressions) {
      args.push_back(evaluate(arg));
    }
  }

  try {
    auto callable = std::any_cast<std::shared_ptr<LoxCallable>>(callee);
    if (args.size() != callable->arity()) {
      throw RuntimeError(expr->paren, "Invalid argument number: arg number = " +
                                          std::to_string(args.size()) +
                                          " function arity = " +
                                          std::to_string(callable->arity()));
    }
    return callable->call(this, args);
  } catch (std::bad_any_cast&) {
    throw RuntimeError(expr->paren, "Can only call functions and classes.");
  }
}

std::any Interpreter::visit(
    std::shared_ptr<const lox::parser::StatementExpression> stmt) {
  evaluate(stmt->expression);
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Print> stmt) {
  std::cout << lox::util::any_to_string(evaluate(stmt->expression));
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Var> stmt) {
  std::any value = stmt->initializer ? evaluate(stmt->initializer) : nullptr;
  env_->define(stmt->token.lexeme, value);
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Block> stmt) {
  execute(stmt->statements, std::make_shared<Environment>(this->env_));
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::If> stmt) {
  if (isTruthy(evaluate(stmt->predicate))) {
    execute(stmt->then);
  } else if (stmt->alternative) {
    execute(stmt->alternative);
  }
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::While> stmt) {
  while (isTruthy(evaluate(stmt->condition))) {
    try {
      execute(stmt->body);
    } catch (Continue&) {
      continue;
    } catch (Break&) {
      break;
    }
  }
  return nullptr;
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Continue> stmt) {
  throw Continue(stmt->token);
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Break> stmt) {
  throw Break(stmt->token);
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Return> stmt) {
  std::any return_value = nullptr;
  if (stmt->value) {
    return_value = stmt->value->accept(this);
  }
  throw Return(stmt->token, return_value);
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Lambda> expr) {
  auto function = std::make_shared<LoxFunction>(expr->function, env_);
  return std::make_any<std::shared_ptr<LoxCallable>>(function);
}

std::any Interpreter::visit(std::shared_ptr<const lox::parser::Function> stmt) {
  auto function = std::make_shared<LoxFunction>(stmt, env_);
  auto callable = std::make_any<std::shared_ptr<LoxCallable>>(function);
  env_->define(stmt->name.lexeme, callable);
  return nullptr;
}

std::any Interpreter::evaluate(
    const std::shared_ptr<lox::parser::Expression>& expr) {
  return expr->accept(this);
}

void Interpreter::execute(const std::shared_ptr<lox::parser::Statement>& stmt) {
  stmt->accept(this);
}

void Interpreter::execute(
    const std::vector<std::shared_ptr<lox::parser::Statement>>& statements,
    std::shared_ptr<Environment> env) {
  auto previous = this->env_;
  try {
    this->env_ = env;
    for (auto stmt : statements) {
      if (stmt) {
        execute(stmt);
      }
    }
  } catch (Return& return_exception) {
    this->env_ = previous;
    throw return_exception;
  } catch (RuntimeError& error) {
    this->env_ = previous;
    throw error;
  }
  this->env_ = previous;
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
  if (object_type == typeid(double)) {
    return std::any_cast<double>(object) != 0;
  }
  if (object_type == typeid(std::string)) {
    return std::any_cast<std::string>(object) != "";
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
    if (left_type == typeid(nullptr)) {
      return true;
    }
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