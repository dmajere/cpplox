#include "Resolver.h"

#include <iostream>

#include "lox.h"

namespace lox {
namespace lang {

Resolver::Resolver(std::shared_ptr<Interpreter> interpreter)
    : interpreter_{std::move(interpreter)} {};

void Resolver::resolve(
    const std::vector<std::shared_ptr<lox::parser::Statement>>& statements) {
  for (const auto& stmt : statements) {
    if (stmt) {
      resolve(stmt);
    }
  }
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Variable> expr) {
  if (!scopes_.empty()) {
    auto& scope = scopes_.back();
    auto it = scope.find(expr->token.lexeme);
    if (it != scope.end() && it->second == false) {
      lox::lang::Lox::error(
          expr->token, "Can't read local variable in it's own initializer.");
    }
  }
  resolve(expr, expr->token);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Assignment> expr) {
  resolve(expr->target);
  resolve(expr, expr->token);
  return nullptr;
}
std::any Resolver::visit(std::shared_ptr<const lox::parser::Binary> expr) {
  resolve(expr->left);
  resolve(expr->right);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Grouping> expr) {
  resolve(expr->expression);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Unary> expr) {
  resolve(expr->right);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Literal> expr) {
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Call> expr) {
  resolve(expr->callee);
  if (expr->arguments) {
    resolve(expr->arguments);
  }
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Sequence> expr) {
  for (const auto ex : expr->expressions) {
    if (ex) {
      ex->accept(this);
    }
  }
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Ternary> expr) {
  resolve(expr->predicate);
  resolve(expr->then);
  if (expr->alternative) {
    resolve(expr->alternative);
  }
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Lambda> expr) {
  resolve((std::shared_ptr<const lox::parser::Function>)expr->function);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Block> stmt) {
  beginScope();
  resolve(stmt->statements);
  endScope();
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Var> stmt) {
  declare(stmt->token);
  if (stmt->initializer) {
    resolve(stmt->initializer);
  }
  define(stmt->token);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Function> stmt) {
  declare(stmt->name);
  define(stmt->name);
  resolve(stmt);
  return nullptr;
}

std::any Resolver::visit(
    std::shared_ptr<const lox::parser::StatementExpression> stmt) {
  resolve(stmt->expression);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Print> stmt) {
  resolve(stmt->expression);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Continue> stmt) {
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Break> stmt) {
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::If> stmt) {
  resolve(stmt->predicate);
  resolve(stmt->then);
  if (stmt->alternative) {
    resolve(stmt->alternative);
  }
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::While> stmt) {
  resolve(stmt->condition);
  resolve(stmt->body);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Return> stmt) {
  if (stmt->value) {
    resolve(stmt->value);
  }
  return nullptr;
}

void Resolver::resolve(const std::shared_ptr<lox::parser::Statement>& stmt) {
  stmt->accept(this);
}

void Resolver::resolve(const std::shared_ptr<lox::parser::Expression>& expr) {
  expr->accept(this);
}

void Resolver::resolve(std::shared_ptr<const lox::parser::Expression> expr,
                       const lox::parser::Token& name) {
  for (int i = scopes_.size() - 1; i >= 0; i--) {
    auto& scope = scopes_.at(i);
    auto it = scope.find(name.lexeme);
    if (it != scope.end()) {
      interpreter_->resolve(expr, scopes_.size() - 1 - i);
      return;
    }
  }
}

void Resolver::resolve(std::shared_ptr<const lox::parser::Function> func) {
  beginScope();
  for (const auto& param : func->parameters) {
    declare(param);
    define(param);
  }
  resolve(func->body);
  endScope();
}

void Resolver::beginScope() {
  scopes_.push_back(std::unordered_map<std::string, bool>{});
}

void Resolver::endScope() { scopes_.pop_back(); }

void Resolver::declare(const lox::parser::Token& name) {
  if (scopes_.empty()) {
    return;
  }
  auto& scope = scopes_.back();
  scope.insert({name.lexeme, false});
}

void Resolver::define(const lox::parser::Token& name) {
  if (scopes_.empty()) {
    return;
  }
  auto& scope = scopes_.back();
  scope.insert_or_assign(name.lexeme, true);
}
}  // namespace lang
}  // namespace lox