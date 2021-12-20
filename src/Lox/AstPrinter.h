#pragma once
#include <any>
#include <string>
#include "expression.h"

namespace lox {
namespace parser {

class AstPrinter : public AstVisitor {
    public:

    void print(Expression* expr);
    std::any visit(const Binary* binary) override;
    std::any visit(const Grouping* grouping) override;
    std::any visit(const Unary* unary) override;
    std::any visit(const Literal* literal) override;
    std::any visit(const Block* block) override;
    std::any visit(const Condition* condition) override;
};

}
}
