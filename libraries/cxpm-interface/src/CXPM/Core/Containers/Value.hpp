#pragma once

#include "CXPM/Core/Containers/BasicCollection.hpp"
#include "CXPM/Core/Containers/Map.hpp"
#include "CXPM/Core/Containers/Variant.hpp"
#include "CXPM/Modules/Serialization/ValueToken.hpp"

namespace CXPM::Core::Containers {

struct Value : public Variant<std::nullptr_t, bool, int, double, String,
                              BasicCollection<Value>, Map<String, Value>> {
  using BaseType = Variant<std::nullptr_t, bool, int, double, String,
                           BasicCollection<Value>, Map<String, Value>>;
  Value() = default;
  template <typename... ArgsTypes>
  Value(ArgsTypes... args) : BaseType(args...) {}
};

} // namespace Core::Containers