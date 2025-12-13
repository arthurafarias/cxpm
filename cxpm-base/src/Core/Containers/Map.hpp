#pragma once

#include "Core/Object.hpp"
#include <map>
template <typename... ArgsTypes>
class Map : public std::map<ArgsTypes...>, public Core::Object {
public:
  using std::map<ArgsTypes...>::map;
};