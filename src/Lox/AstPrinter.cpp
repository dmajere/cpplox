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

std::any AstPrinter::visit(std::shared_ptr<const Binary> binary) {
  std::stringstream ss;
  ss << "( " << binary->op.lexeme << " "
     << std::any_cast<std::string>(binary->left->accept(this)) << " "
     << std::any_cast<std::string>(binary->right->accept(this)) << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Grouping> grouping) {
  std::stringstream ss;
  ss << "( " << std::any_cast<std::string>(grouping->expression->accept(this))
     << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Unary> unary) {
  std::stringstream ss;
  ss << "( " << unary->op.lexeme << " "
     << std::any_cast<std::string>(unary->right->accept(this)) << " )";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Literal> literal) {
  auto& value_type = literal->value.type();
  if (value_type == typeid(nullptr)) {
    return std::string{"nil"};
  } else if (value_type == typeid(std::string)) {
    return "\"" + std::any_cast<std::string>(literal->value) + "\"";
  } else if (value_type == typeid(double)) {
    return std::to_string(std::any_cast<double>(literal->value));
  } else if (value_type == typeid(bool)) {
    return std::string{std::any_cast<bool>(literal->value) ? "true" : "false"};
  }
  return "";
}

std::any AstPrinter::visit(std::shared_ptr<const Variable> var) {
  return var->token.lexeme;
}

std::any AstPrinter::visit(std::shared_ptr<const Block> block) {
  std::stringstream ss;
  ss << "( ";
  // for (int i = 0; i <= block->expressions.size(); i++) {
  for (auto& expr : block->expressions) {
    // ss << std::any_cast<std::string>(block->expressions[i]->accept(this))
    ss << std::any_cast<std::string>(expr->accept(this)) << ", ";
  }
  ss << ")";
  return ss.str();
}

std::any AstPrinter::visit(std::shared_ptr<const Condition> condition) {
  std::stringstream ss;
  ss << "(if ("
     << std::any_cast<std::string>(condition->predicate->accept(this)) << ") "
     << std::any_cast<std::string>(condition->then->accept(this));
  if (condition->alternative) {
    ss << " else "
       << std::any_cast<std::string>(condition->alternative->accept(this));
  }
  ss << " )";
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
  ss << "(var " << stmt->token.lexeme;
  if (stmt->initializer) {
    ss << " = " << std::any_cast<std::string>(stmt->initializer->accept(this));
  }
  ss << ")";
  return ss.str();
}

}  // namespace parser
}  // namespace lox
