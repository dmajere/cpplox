#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
#include "AstPrinter.h"

namespace lox {
namespace parser {

void AstPrinter::print(Expression* expr) {
    if (expr) {
        std::cout << std::any_cast<std::string>(expr->accept(this));
    }
}

std::any AstPrinter::visit(const Binary* binary) {
    std::stringstream ss;
    ss << "( " << binary->op.lexeme
       << " " << std::any_cast<std::string>(binary->left->accept(this))
       << " " << std::any_cast<std::string>(binary->right->accept(this))
       << " )";
    return ss.str();
}

std::any AstPrinter::visit(const Grouping* grouping) {
    std::stringstream ss;
    ss << "( " 
       << std::any_cast<std::string>(grouping->expression->accept(this))
       << " )";
    return ss.str();
}

std::any AstPrinter::visit(const Unary* unary) {
    std::stringstream ss;
    ss << "( " << unary->op.lexeme
       << " " << std::any_cast<std::string>(unary->right->accept(this))
       << " )";
    return ss.str();
}

std::any AstPrinter::visit(const Literal* literal) {
    auto& value_type = literal->value.type();
    if (value_type == typeid(nullptr)) {
        return std::string{"nil"};
    } else if (value_type == typeid(std::string)) {
        return std::any_cast<std::string>(literal->value);
    } else if (value_type == typeid(double)) {
        return std::to_string(std::any_cast<double>(literal->value));
    } else if (value_type == typeid(bool)) {
        return std::string{std::any_cast<bool>(literal->value) ? "true" : "false"};
    }
    return "";
}

std::any AstPrinter::visit(const Block* block) {
    std::stringstream ss;
    ss << "( ";
    // for (int i = 0; i <= block->expressions.size(); i++) {
    for (auto& expr : block->expressions) {
        // ss << std::any_cast<std::string>(block->expressions[i]->accept(this)) 
        ss << std::any_cast<std::string>(expr->accept(this)) 
           << ", ";
    }
    ss << ")";
    return ss.str();
}

std::any AstPrinter::visit(const Condition* condition) {
    std::stringstream ss;
    ss << "(if ("
       << std::any_cast<std::string>(condition->predicate->accept(this)) 
       << ") "
       << std::any_cast<std::string>(condition->then->accept(this));
    if (condition->alternative) {
        ss << " else " << std::any_cast<std::string>(condition->alternative->accept(this)); 
    }
    ss << " )";
    return ss.str();
}

}
}

