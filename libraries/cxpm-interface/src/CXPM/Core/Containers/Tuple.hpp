#pragma once

#include "CXPM/Core/Object.hpp"
#include "CXPM/Core/Threading/UniqueLock.hpp"
#include <deque>

namespace Core::Containers {
template <typename... ArgsTypes> class Tuple : public std::tuple<ArgsTypes...> {
public:
  using std::tuple<ArgsTypes...>::tuple;
};
} // namespace Core::Containers