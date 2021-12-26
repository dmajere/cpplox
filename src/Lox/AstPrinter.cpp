#include "AstPrinter.h"

#include <sstream>
#include <string>

#include "utils.h"

namespace lox {
namespace parser {

std::string AstPrinter::print(const std::shared_ptr<Expression>& expr) {
  return lox::util::any_to_string(expr->accept(this));
}
std::string AstPrinter::print(const std::shared_ptr<Statement>& stmt) {
  return lox::util::any_to_string(stmt->accept(this));
}

std::any AstPrinter::visit(std::shared_ptr<const Binary> expr) {
  std::stringstream ss;
  ss << "( " << expr->op.lexeme << " "
     << lox::util::any_to_string(expr->left->accept(this)) << " "
     << lox::util::any_to_string(expr->right->accept(this)) << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Grouping> expr) {
  std::stringstream ss;
  ss << "(" << lox::util::any_to_string(expr->expression->accept(this)) << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Unary> expr) {
  std::stringstream ss;
  ss << "(" << expr->op.lexeme << " "
     << lox::util::any_to_string(expr->right->accept(this)) << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Literal> expr) {
  return lox::util::any_to_string(expr->value);
}

std::any AstPrinter::visit(std::shared_ptr<const Variable> expr) {
  return expr->token.lexeme;
}

std::any AstPrinter::visit(std::shared_ptr<const Sequence> expr) {
  std::stringstream ss;
  ss << "(";
  for (const auto& expr : expr->expressions) {
    ss << lox::util::any_to_string(expr->accept(this)) << ", ";
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Ternary> expr) {
  std::stringstream ss;
  ss << "(? (" << lox::util::any_to_string(expr->predicate->accept(this))
     << ") " << lox::util::any_to_string(expr->then->accept(this));
  if (expr->alternative) {
    ss << " : " << lox::util::any_to_string(expr->alternative->accept(this));
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Assignment> expr) {
  std::stringstream ss;
  ss << "(assign " << expr->token.lexeme << " "
     << lox::util::any_to_string(expr->target->accept(this)) << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Call> expr) {
  std::stringstream ss;
  ss << "(call " << lox::util::any_to_string(expr->callee->accept(this)) << " ";
  if (expr->arguments) {
    ss << lox::util::any_to_string(expr->arguments->accept(this));
  }
  ss << ")";

  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Lambda> expr) {
  std::stringstream ss;
  ss << "(lambda (";
  if (!expr->function->parameters.empty()) {
    for (const auto& p : expr->function->parameters) {
      ss << p.lexeme << ", ";
    }
  }
  ss << ") ";
  if (expr->function->body) {
    ss << lox::util::any_to_string(expr->function->body->accept(this));
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Get> expr) {
  std::stringstream ss;
  ss << "(get " << lox::util::any_to_string(expr->object->accept(this)) << "->"
     << expr->name.lexeme << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Set> expr) {
  std::stringstream ss;
  ss << "(set " << lox::util::any_to_string(expr->object->accept(this)) << "->"
     << expr->name.lexeme << lox::util::any_to_string(expr->value->accept(this))
     << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const StatementExpression> stmt) {
  std::stringstream ss;
  ss << "(" << lox::util::any_to_string(stmt->expression->accept(this)) << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Print> stmt) {
  std::stringstream ss;
  ss << "(print " << lox::util::any_to_string(stmt->expression->accept(this))
     << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Var> stmt) {
  std::stringstream ss;
  ss << "(define " << stmt->token.lexeme;
  if (stmt->initializer) {
    ss << " = " << lox::util::any_to_string(stmt->initializer->accept(this));
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Block> stmt) {
  std::stringstream ss;
  ss << "{ \n";
  for (const auto& s : stmt->statements) {
    ss << lox::util::any_to_string(s->accept(this)) << "\n";
  }
  ss << "}";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const If> stmt) {
  std::stringstream ss;
  ss << "(if (" << lox::util::any_to_string(stmt->predicate->accept(this))
     << ") " << lox::util::any_to_string(stmt->then->accept(this));
  if (stmt->alternative) {
    ss << " else " << lox::util::any_to_string(stmt->alternative->accept(this));
  }
  ss << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const While> stmt) {
  std::stringstream ss;
  ss << "(while (" << lox::util::any_to_string(stmt->condition->accept(this))
     << ") {" << lox::util::any_to_string(stmt->body->accept(this)) << "}";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Continue> stmt) {
  return stmt->token.lexeme;
}
std::any AstPrinter::visit(std::shared_ptr<const Break> stmt) {
  return stmt->token.lexeme;
}
std::any AstPrinter::visit(std::shared_ptr<const Return> stmt) {
  return stmt->token.lexeme;
}

std::any AstPrinter::visit(std::shared_ptr<const Function> stmt) {
  std::stringstream ss;
  ss << "(fun" << stmt->name.lexeme << "(";
  if (!stmt->parameters.empty()) {
    for (const auto& p : stmt->parameters) {
      ss << p.lexeme << ", ";
    }
  }
  ss << ") ";
  if (stmt->body) {
    ss << lox::util::any_to_string(stmt->body->accept(this));
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Class> stmt) {
  std::stringstream ss;
  ss << "(class " << stmt->name.lexeme << " {";
  if (!stmt->methods.empty()) {
    for (const auto& method : stmt->methods) {
      if (method) {
        ss << lox::util::any_to_string(method->accept(this));
      }
    }
  }
  ss << "})";
  return ss.str();
}

}  // namespace parser
}  // namespace lox
