#pragma once

#include "Core/SharedPointer.hpp"
#include <initializer_list>
#include <utility>
namespace Utils::Patterns {

template <typename DerivedType> class InitializerListCreator {
public:
  template <typename InitializerType>
  static SharedPointer<DerivedType>
  create(const std::initializer_list<InitializerType> &list) {
    return SharedPointer<DerivedType>(
        new DerivedType(list.begin(), list.end()));
  }
};

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