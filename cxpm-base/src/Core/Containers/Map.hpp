#pragma once

#include "Core/Object.hpp"
#include <map>
template <typename... ArgsTypes>
class Map : public std::map<ArgsTypes...>, public Core::Object {
public:
  using std::map<ArgsTypes...>::map;

  template <typename... ConstructorArguments>
  Map(ConstructorArguments... args)
      : std::map<ArgsTypes...>(
            std::forward<ConstructorArguments>(args)...) {}

  template <typename... ConstructorArguments>
  Map(const ConstructorArguments &...args)
      : std::map<ArgsTypes...>(
            std::forward<const ConstructorArguments &>(args)...) {}

  template <typename... ConstructorArguments>
  Map(const ConstructorArguments &&...args)
      : std::map<ArgsTypes...>(
            std::forward<const ConstructorArguments &&>(args)...) {}
};