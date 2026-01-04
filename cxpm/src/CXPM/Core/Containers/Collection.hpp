#pragma once

#include <deque>
#include <functional>
#include <utility>

namespace CXPM::Core::Containers {
template <typename ContainedType>
class Collection : public std::deque<ContainedType> {
public:
  using std::deque<ContainedType>::deque;

  template <typename ReturnType>
  Collection<ReturnType>
  transform(std::function<ReturnType(const ContainedType &)> transfomer) const {
    Collection<ReturnType> retval;

    for (auto el : *this) {
      retval.push_back(transfomer(el));
    }

    return std::move(retval);
  }
};
} // namespace CXPM::Core::Containers