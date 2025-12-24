#pragma once

#include <functional>

template <typename... ArgsTypes>
class Function : public std::function<ArgsTypes...> {
public:
  using std::function<ArgsTypes...>::function;
};