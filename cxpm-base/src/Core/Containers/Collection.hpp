#pragma once

#include "Core/Object.hpp"
#include <deque>
#include <functional>
#include <initializer_list>
#include <utility>

namespace Core::Containers {
template <typename ContainedType>
class Collection : public std::deque<ContainedType>, public Object {
public:
  Collection(const std::initializer_list<ContainedType> &list)
      : std::deque<ContainedType>(list) {}
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

  Collection<ContainedType>
  slice(const Collection<ContainedType>::iterator &begin,
        const Collection<ContainedType>::iterator &end) {
    Collection<ContainedType> sliced;
    auto el = begin;
    while (el != end) {
      sliced.push_back(*el++);
    }
  }
};
} // namespace Core::Containers