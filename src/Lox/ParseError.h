#pragma once
#include <stdexcept>

namespace lox {
namespace parser {
struct ParseError : public std::runtime_error {
  using std::runtime_error::runtime_error;
};

}  // namespace parser
}  // namespace lox