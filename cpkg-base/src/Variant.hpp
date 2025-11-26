#pragma once

#include <deque>
#include <iomanip>
#include <string>
#include <type_traits>
#include <variant>

#include <Utils/Print.hpp>

class Variant;
class Variant
    : public std::variant<int, double, std::string, std::deque<Variant>> {
public:
  using BaseType = std::variant<int, double, std::string, std::deque<Variant>>;
  using std::variant<int, double, std::string, std::deque<Variant>>::variant;

  template <typename Archiver> void constexpr serialize(const Archiver &ar) {
    using type = std::decay_t<decltype(ar)>();
    ar & static_cast<type>(*this);
  }
  
};

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
inline std::ostream &print<const std::string &>(std::ostream &os,
                                                const std::string &value) {
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