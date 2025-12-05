#pragma once

#include <functional>
#include <tuple>

template <typename... ArgsTypes>
class Function : public std::function<ArgsTypes...> {
public:
  using std::function<ArgsTypes...>::function;
};