#pragma once
#include <any>
#include <string>

#include "expression.h"

namespace lox {
namespace parser {

class AstPrinter : public AstVisitor {
 public:
  std::string print(std::shared_ptr<Expression> expr);
  std::any visit(std::shared_ptr<const Binary> binary) override;
  std::any visit(std::shared_ptr<const Grouping> grouping) override;
  std::any visit(std::shared_ptr<const Unary> unary) override;
  std::any visit(std::shared_ptr<const Literal> literal) override;
  std::any visit(std::shared_ptr<const Block> block) override;
  std::any visit(std::shared_ptr<const Condition> condition) override;
};

}  // namespace parser
}  // namespace lox
