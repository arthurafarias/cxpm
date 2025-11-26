#pragma once

#include <ostream>

namespace Utils {
template <typename ArgumentType>
std::ostream &print(std::ostream &os, ArgumentType workspace);

template <>
inline std::ostream &print<const char *>(std::ostream &os, const char *value) {
  os << value;
  return os;
}
} // namespace Utils::Print