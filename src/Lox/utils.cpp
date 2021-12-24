#include "utils.h"

namespace lox {
namespace util {

std::string any_to_string(const std::any& object) {
  auto& object_type = object.type();

  if (!object.has_value()) {
    return "";
  }
  if (object_type == typeid(nullptr)) {
    return "nil";
  } else if (object_type == typeid(std::string)) {
    return std::any_cast<std::string>(object);
  } else if (object_type == typeid(double)) {
    return std::to_string(std::any_cast<double>(object));
  } else if (object_type == typeid(bool)) {
    return std::string{std::any_cast<bool>(object) ? "true" : "false"};
  }

  return "";
}
}
}  // namespace lox