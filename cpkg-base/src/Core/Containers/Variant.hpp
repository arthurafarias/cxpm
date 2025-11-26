#pragma once

#include <Core/Containers/String.hpp>

#include <Utils/Print.hpp>

#include <deque>
#include <iomanip>
#include <string>
#include <type_traits>
#include <variant>

namespace Core::Containers {
class Variant;
class Variant
    : public std::variant<int, double, String, std::deque<Variant>> {
public:
  using BaseType = std::variant<int, double, String, std::deque<Variant>>;
  using std::variant<int, double, String, std::deque<Variant>>::variant;

  template <typename Archiver> void constexpr serialize(const Archiver &ar) {
    using type = std::decay_t<decltype(ar)>();
    ar &static_cast<type>(*this);
  }
};
} // namespace Core::Containers

namespace Utils {

using namespace Core::Containers;

template <>
inline std::ostream &print<Variant>(std::ostream &os, Variant values) {
  return os;
}

template <>
inline std::ostream &print<const int &>(std::ostream &os, const int &value) {
  os << value;
  return os;
}

template <>
inline std::ostream &print<const double &>(std::ostream &os,
                                           const double &value) {
  os << value;
  return os;
}

template <>
inline std::ostream &print<const String &>(std::ostream &os,
                                                const String &value) {
  os << std::quoted(value);
  return os;
}

template <>
inline std::ostream &print<std::deque<Variant>>(std::ostream &os,
                                                std::deque<Variant> values) {

  auto _values_to_print = values;

  print(os, _values_to_print.front());
  _values_to_print.pop_front();

  while (!_values_to_print.empty()) {
    print(os, ",");
    print(os, _values_to_print.front());
  }

  return os;
}

template <>
inline std::ostream &
print<const std::deque<Variant> &>(std::ostream &os,
                                   const std::deque<Variant> &values) {

  auto _values_to_print = values;

  print(os, _values_to_print.front());
  _values_to_print.pop_front();

  while (!_values_to_print.empty()) {
    print(os, ",");
    print(os, _values_to_print.front());
  }

  return os;
}
} // namespace Utils::Print