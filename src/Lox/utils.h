#pragma once

#include <any>
#include <string>
#include <type_traits>

namespace lox {
namespace util {

std::string any_to_string(const std::any& object);

}  // namespace util

}  // namespace lox