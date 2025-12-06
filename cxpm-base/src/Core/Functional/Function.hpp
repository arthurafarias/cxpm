#pragma once

#include <functional>
#include <tuple>

namespace Core::Functional {
template <typename... ArgsTypes>
class Function : public std::function<ArgsTypes...> {
public:
  using std::function<ArgsTypes...>::function;
};
} // namespace Core::Functional