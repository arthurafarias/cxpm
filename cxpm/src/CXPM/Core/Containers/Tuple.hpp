#pragma once

#include <CXPM/Core/Object.hpp>
#include <CXPM/Core/Threading/UniqueLock.hpp>
#include <deque>

namespace CXPM::Core::Containers {
template <typename... ArgsTypes> class Tuple : public std::tuple<ArgsTypes...> {
public:
  using std::tuple<ArgsTypes...>::tuple;
};
} // namespace CXPM::Core::Containers