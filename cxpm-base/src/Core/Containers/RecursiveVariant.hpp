#pragma once

#include "Core/Containers/Variant.hpp"
#include <Core/Containers/String.hpp>

#include <Utils/Print.hpp>

#include <deque>
#include <iomanip>
#include <string>
#include <type_traits>
#include <variant>

using namespace Core::Containers;

namespace Core::Containers {

template <typename... ArgsTypes> class RecursiveVariant;

template <typename... ArgsTypes>
class RecursiveVariant
    : public Variant<RecursiveVariant<ArgsTypes...>, ArgsTypes...> {
public:
  using Variant<RecursiveVariant<ArgsTypes...>, ArgsTypes...>::variant;
  using base_type = Variant<RecursiveVariant<ArgsTypes...>, ArgsTypes...>;
};
} // namespace Core::Containers