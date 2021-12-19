#include <iostream>
#include <sstream>
#include "AstPrinter.h"

namespace lox {
namespace parser {


void AstPrinter::visit(const Binary* binary) {
    std::cout << binary->toString() << "\n";
}
void AstPrinter::visit(const Grouping* grouping) {
    std::cout << grouping->toString() << "\n";
}
void AstPrinter::visit(const Literal* literal) {
    std::cout << literal->toString() << "\n";
}
void AstPrinter::visit(const Unary* unary) {
    std::cout << unary->toString() << "\n";
}

const std::string AstPrinter::get() const {
    return line_;
}
void AstPrinter::save(const std::string& str) {
    this->line_ = str;
}

}
}

