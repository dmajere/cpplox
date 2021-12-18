#include <string_view>
#include <folly/File.h>
#include <folly/FileUtil.h>

#include "interpreter.h"
#include "scanner.h"

constexpr std::string_view kInterpreterInputPrompt = " ]=> ";

namespace lox {
namespace lang {

bool Interpreter::hadError = false;

void Interpreter::runFromFile(const std::string& path) {
    auto f = folly::File(path);
    std::string code;
    folly::readFile(f.fd(), code);
    run(code);
    
}

void Interpreter::runPrompt() {
    for (std::string line; ; std::getline(std::cin, line)) {
        if (std::cin.fail()) {
            return;
        }
        // TODO: result is expression
        // print output prompt + expression.repr()
        if (!line.empty()) {
            run(line);
            std::cout << "\n";
        }
        if (hadError) {
            return;
        }
        std::cout << kInterpreterInputPrompt;
    }
}

void Interpreter::run(const std::string& source) {
    auto scanner = lox::parser::Scanner(source);
    auto tokens = scanner.scanTokens();
    for (const lox::parser::Token& t: tokens) {
        std::cout << t << "\n"; 
    }
}

}
}