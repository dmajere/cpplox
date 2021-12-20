#pragma once
#include "expression.h"

namespace lox {
namespace parser {

class AstPrinter : public AstVisitor {
    public:

    void print(Expression* expr) {
        expr->accept(this);
    }
    void visit(const Binary* binary) override;
    void visit(const Grouping* grouping) override;
    void visit(const Literal* literal) override;
    void visit(const Unary* unary) override;
    const std::string get() const;

    private:
    void save(const std::string& str);
    std::string line_;
};

}
}
