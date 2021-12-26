#include "Resolver.h"

#include <string_view>

#include "ParseError.h"
#include "lox.h"

constexpr std::string_view kVariableInInitializer =
    "Can't read local variable in it's own initializer.";
constexpr std::string_view kVariableDefined = "Variable already defined.";

namespace lox {
namespace lang {

Resolver::Resolver(std::shared_ptr<Interpreter> interpreter)
    : interpreter_{std::move(interpreter)},
      currentFunction_(FunctionType::None) {
  beginScope();
};
Resolver::~Resolver() { endScope(); }

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
      lox::lang::Lox::error(expr->token, std::string(kVariableInInitializer));
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
  resolve(expr->function, FunctionType::Function);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Get> expr) {
  resolve(expr->object);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Set> expr) {
  resolve(expr->object);
  resolve(expr->value);
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
  resolve(stmt, FunctionType::Function);
  return nullptr;
}

std::any Resolver::visit(std::shared_ptr<const lox::parser::Class> stmt) {
  declare(stmt->name);
  define(stmt->name);
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
  if (currentFunction_ == FunctionType::None) {
    lox::lang::Lox::error(stmt->token, "Return not inside function.");
  }
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
  for (int i = scopes_.size() - 1; i >= 1; i--) {
    auto& scope = scopes_.at(i);
    auto it = scope.find(name.lexeme);
    if (it != scope.end()) {
      interpreter_->resolve(expr, scopes_.size() - 1 - i);
      return;
    }
  }
}

void Resolver::resolve(std::shared_ptr<const lox::parser::Function> func,
                       FunctionType type) {
  FunctionType enclosing = currentFunction_;
  currentFunction_ = type;
  beginScope();
  for (const auto& param : func->parameters) {
    declare(param);
    define(param);
  }
  resolve(func->body);
  endScope();
  currentFunction_ = enclosing;
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
  if (scope.find(name.lexeme) != scope.end()) {
    lox::lang::Lox::error(name, std::string(kVariableDefined));
  }
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