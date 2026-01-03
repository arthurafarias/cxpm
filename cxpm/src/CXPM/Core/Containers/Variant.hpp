#pragma once

#include <CXPM/Core/Containers/String.hpp>

#include <CXPM/Utils/Print.hpp>

#include <variant>

using namespace CXPM::Core::Containers;

namespace CXPM::Core::Containers {
template <typename... ArgsTypes> class Variant : public std::variant<ArgsTypes...> {
public:
  using std::variant<ArgsTypes...>::variant;
  using base_type = std::variant<ArgsTypes...>;
};
} // namespace CXPM::Core::Containers