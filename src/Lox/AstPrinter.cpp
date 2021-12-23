#include "AstPrinter.h"

#include <sstream>
#include <string>
#include <type_traits>

namespace lox {
namespace parser {

std::string AstPrinter::print(std::shared_ptr<Expression> expr) {
  if (expr) {
    return std::any_cast<std::string>(expr->accept(this));
  }
  return "";
}
std::string AstPrinter::print(std::shared_ptr<Statement> stmt) {
  if (stmt) {
    return std::any_cast<std::string>(stmt->accept(this));
  }
  return "";
}

std::any AstPrinter::visit(std::shared_ptr<const Binary> expr) {
  std::stringstream ss;
  ss << "( " << expr->op.lexeme << " "
     << std::any_cast<std::string>(expr->left->accept(this)) << " "
     << std::any_cast<std::string>(expr->right->accept(this)) << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Grouping> expr) {
  std::stringstream ss;
  ss << "( " << std::any_cast<std::string>(expr->expression->accept(this))
     << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Unary> expr) {
  std::stringstream ss;
  ss << "( " << expr->op.lexeme << " "
     << std::any_cast<std::string>(expr->right->accept(this)) << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Literal> expr) {
  auto& value_type = expr->value.type();
  if (value_type == typeid(nullptr)) {
    return std::string{"nil"};
  } else if (value_type == typeid(std::string)) {
    return "\"" + std::any_cast<std::string>(expr->value) + "\"";
  } else if (value_type == typeid(double)) {
    return std::to_string(std::any_cast<double>(expr->value));
  } else if (value_type == typeid(bool)) {
    return std::string{std::any_cast<bool>(expr->value) ? "true" : "false"};
  }
  return "";
}

std::any AstPrinter::visit(std::shared_ptr<const Variable> expr) {
  return expr->token.lexeme;
}

std::any AstPrinter::visit(std::shared_ptr<const Sequence> expr) {
  std::stringstream ss;
  ss << "( ";
  for (auto& expr : expr->expressions) {
    ss << std::any_cast<std::string>(expr->accept(this)) << ", ";
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Ternary> expr) {
  std::stringstream ss;
  ss << "(? (" << std::any_cast<std::string>(expr->predicate->accept(this))
     << ") " << std::any_cast<std::string>(expr->then->accept(this));
  if (expr->alternative) {
    ss << " : "
       << std::any_cast<std::string>(expr->alternative->accept(this));
  }
  ss << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Assignment> expr) {
  std::stringstream ss;
  ss << "(assign " << expr->token.lexeme << " "
     << std::any_cast<std::string>(expr->target->accept(this)) << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Call> expr) {
  std::stringstream ss;
  ss << "(call "
    << std::any_cast<std::string>(expr->callee->accept(this)) << " ";
  if (expr->arguments) {
    ss << std::any_cast<std::string>(expr->arguments->accept(this)) ; 
  }
  ss << ")";

  return ss.str();
}


std::any AstPrinter::visit(std::shared_ptr<const StatementExpression> stmt) {
  std::stringstream ss;
  ss << "(" << std::any_cast<std::string>(stmt->expression->accept(this))
     << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Print> stmt) {
  std::stringstream ss;
  ss << "(print " << std::any_cast<std::string>(stmt->expression->accept(this))
     << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Var> stmt) {
  std::stringstream ss;
  ss << "(define " << stmt->token.lexeme;
  if (stmt->initializer) {
    ss << " = " << std::any_cast<std::string>(stmt->initializer->accept(this));
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Block> stmt) {
  std::stringstream ss;
  ss << "{ \n";
  for (auto& s : stmt->statements) {
    ss << std::any_cast<std::string>(s->accept(this)) << "\n";
  }
  ss << "}";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const If> stmt) {
  std::stringstream ss;
  ss << "(if (" << std::any_cast<std::string>(stmt->predicate->accept(this))
     << ") " << std::any_cast<std::string>(stmt->then->accept(this));
  if (stmt->alternative) {
    ss << " else "
       << std::any_cast<std::string>(stmt->alternative->accept(this));
  }
  ss << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const While> stmt) {
  std::stringstream ss;
  ss << "(while ("
     << std::any_cast<std::string>(stmt->condition->accept(this)) 
     << ") {"
     << std::any_cast<std::string>(stmt->body->accept(this)) 
     << "}";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const LoopControl> stmt) {
  return stmt->token.lexeme;
}

std::any AstPrinter::visit(std::shared_ptr<const Function> stmt) {
  std::stringstream ss;
  ss << "(fun" << stmt->name.lexeme << "(";
  if (stmt->parameters.size()) {
    for (const auto& p : stmt->parameters) {
      ss << p.lexeme << ", ";
    }
  }
  ss << ") {";
  if (stmt->body.size()) {
    for (const auto& expr : stmt->body) {
      ss << std::any_cast<std::string>(expr->accept(this));
    }
  }
  ss  << "})"; 
  return ss.str();
}

}  // namespace parser
}  // namespace lox
