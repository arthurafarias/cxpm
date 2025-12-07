#pragma once

#include "Core/SharedPointer.hpp"
#include <utility>
namespace Utils::Patterns {
template <typename DerivedType> class Creator {
public:
  template <typename... DerivedConstructorArgumentsTypes>
  static SharedPointer<DerivedType>
  create(DerivedConstructorArgumentsTypes... args) {
    return SharedPointer<DerivedType>(new DerivedType(
        std::forward<DerivedConstructorArgumentsTypes>(args)...));
  }
};
} // namespace Utils::Patterns