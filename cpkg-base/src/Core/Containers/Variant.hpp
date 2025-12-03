#pragma once

#include <Core/Containers/String.hpp>

#include <Utils/Print.hpp>

#include <deque>
#include <iomanip>
#include <string>
#include <type_traits>
#include <variant>

using namespace Core::Containers;

namespace Core::Containers {
template <typename... ArgsTypes> class Variant : public std::variant<ArgsTypes...> {
public:
  using std::variant<ArgsTypes...>::variant;
  using base_type = std::variant<ArgsTypes...>;
};
} // namespace Core::Containers